#include <iostream>

#include <clang/AST/Decl.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Lex/Lexer.h>
#include <clang/Tooling/JSONCompilationDatabase.h>
#include <clang/Tooling/Tooling.h>

int main(int argc, char** argv) {
    if (argc != 4) {
        // For now, we expect a compilation database, a filename, and a function.
        std::cerr << "Usage: optimator compile_commands.json filename.cpp function" << std::endl;
        return 1;
    }

    std::string error;
    const auto database = clang::tooling::JSONCompilationDatabase::loadFromFile(
        argv[1], error, clang::tooling::JSONCommandLineSyntax::AutoDetect);
    if (!database) {
        std::cerr << "Could not load compilation database." << std::endl << error << std::endl;
        return 1;
    }

    clang::tooling::ClangTool tool(*database, {argv[2]});
    std::vector<std::unique_ptr<clang::ASTUnit>> ASTs;
    if (tool.buildASTs(ASTs) != 0) {
        std::cerr << "Failed to build the AST." << std::endl;
    }

    assert(ASTs.size() == 1);

    auto& context = ASTs[0]->getASTContext();
    const auto& source = ASTs[0]->getSourceManager();
    const auto& options = ASTs[0]->getLangOpts();

    const auto matcher = clang::ast_matchers::functionDecl(clang::ast_matchers::hasName(argv[3])).bind("root");
    const auto matches = clang::ast_matchers::match(matcher, context);
    for (const auto& match : matches) {
        const auto* root = match.getNodeAs<clang::FunctionDecl>("root");
        const auto range = clang::CharSourceRange::getCharRange(root->getSourceRange());
        std::cout << clang::Lexer::getSourceText(range, source, options).str() << std::endl;
    }
}
