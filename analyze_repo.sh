#!/bin/bash

# ============================================================================
# WSL2 Repository Analysis Script
# Purpose: Gather information about the paging-simulator repository structure
# ============================================================================

echo "======================================"
echo "PAGING SIMULATOR - REPOSITORY ANALYSIS"
echo "======================================"
echo ""

# 1. Navigate to repository (adjust path if needed)
echo "📁 Current directory:"
pwd
echo ""

# 2. Check if we're in a git repository
echo "🔍 Git repository check:"
git rev-parse --is-inside-work-tree 2>/dev/null
echo ""

# 3. Show complete directory structure
echo "📂 COMPLETE DIRECTORY TREE:"
echo "-----------------------------------"
tree -a -L 3 --dirsfirst
# If tree is not installed: sudo apt install tree
echo ""

# Alternative if tree is not available:
echo "📂 ALTERNATIVE: Directory structure with find:"
echo "-----------------------------------"
find . -type f -o -type d | sort
echo ""

# 4. List all files with details
echo "📄 ALL FILES (with sizes):"
echo "-----------------------------------"
ls -lAhR
echo ""

# 5. Count files by type
echo "📊 FILE TYPE SUMMARY:"
echo "-----------------------------------"
find . -type f | sed 's/.*\.//' | sort | uniq -c | sort -rn
echo ""

# 6. Find all C/C++ source files
echo "💻 C/C++ SOURCE FILES:"
echo "-----------------------------------"
find . -type f \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) | sort
echo ""

# 7. Find all documentation files
echo "📝 DOCUMENTATION FILES:"
echo "-----------------------------------"
find . -type f \( -name "*.md" -o -name "*.txt" -o -name "*.rst" -o -name "README*" \) | sort
echo ""

# 8. Check for existing README
echo "📖 CURRENT README CONTENT:"
echo "-----------------------------------"
if [ -f "README.md" ]; then
    cat README.md
elif [ -f "README.txt" ]; then
    cat README.txt
elif [ -f "README" ]; then
    cat README
else
    echo "❌ No README file found"
fi
echo ""

# 9. Check for LICENSE
echo "⚖️ LICENSE FILE:"
echo "-----------------------------------"
if [ -f "LICENSE" ] || [ -f "LICENSE.md" ] || [ -f "LICENSE.txt" ]; then
    ls -lh LICENSE* 2>/dev/null
    echo "✅ License file exists"
else
    echo "❌ No LICENSE file found"
fi
echo ""

# 10. Check for .gitignore
echo "🚫 GITIGNORE STATUS:"
echo "-----------------------------------"
if [ -f ".gitignore" ]; then
    echo "✅ .gitignore exists"
    cat .gitignore
else
    echo "❌ No .gitignore file found"
fi
echo ""

# 11. Check for any existing documentation folders
echo "📚 EXISTING DOC FOLDERS:"
echo "-----------------------------------"
find . -type d \( -name "doc" -o -name "docs" -o -name "documentation" \) 2>/dev/null
echo ""

# 12. Look for milestone-related folders/files
echo "🎯 MILESTONE-RELATED CONTENT:"
echo "-----------------------------------"
find . -type f -o -type d | grep -i milestone | sort
echo ""

# 13. Check for any build artifacts or executables
echo "🔨 EXECUTABLES & BUILD ARTIFACTS:"
echo "-----------------------------------"
find . -type f -executable
echo ""

# 14. Count total lines of code (C files only)
echo "📈 CODE STATISTICS:"
echo "-----------------------------------"
echo "Total C source files:"
find . -name "*.c" -type f | wc -l
echo "Total header files:"
find . -name "*.h" -type f | wc -l
echo "Total lines of C code:"
find . -name "*.c" -type f -exec wc -l {} + | tail -1
echo ""

# 15. Show git status
echo "🔄 GIT STATUS:"
echo "-----------------------------------"
git status
echo ""

# 16. Show last few commits
echo "📜 RECENT COMMITS:"
echo "-----------------------------------"
git log --oneline -10 2>/dev/null || echo "Not a git repository or no commits"
echo ""

# 17. Check for any test files
echo "🧪 TEST FILES:"
echo "-----------------------------------"
find . -type f \( -name "*test*" -o -name "*Test*" \) | sort
echo ""

# 18. Summary
echo "======================================"
echo "✅ ANALYSIS COMPLETE"
echo "======================================"
echo ""
echo "Next steps:"
echo "1. Review the output above"
echo "2. Share this information"
echo "3. We'll create the new structure together"
echo ""
