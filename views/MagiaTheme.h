#ifndef MAGIA_THEME_H
#define MAGIA_THEME_H

namespace mg {
namespace theme {

// Main UI Colors
struct Colors {
    // Main colors
    inline static int CODE_BG = 0x181830;            // #181830 - Darkest blue-purple (for code area)
    inline static int BACKGROUND = 0x1E1E3F;         // #1E1E3F - Dark blue-purple background
    inline static int SIDEBAR_BG = 0x2D2B55;         // #2D2B55 - Sidebar background
    inline static int BLACK = 0x222222;         // #2D2B55 - Sidebar background
    inline static int ACTIVE_ITEM = 0x433C7E;        // #433C7E - Active/selected items
    inline static int PRIMARY = 0x7E57C2;            // #7E57C2 - Primary purple
    inline static int SECONDARY = 0x433C7E;          // #433C7E - Medium purple (same as active)
    inline static int BORDER = 0x433C7E;             // #433C7E - Border color
    
    // Text and syntax colors
    inline static int DEFAULT_TEXT = 0xE0E0E0;       // #E0E0E0 - Light gray text
    inline static int KEYWORD = 0xFF79C6;            // #FF79C6 - Pink (keywords)
    inline static int STRING = 0xF1FA8C;             // #F1FA8C - Yellow (strings)
    inline static int COMMENT = 0x6272A4;            // #6272A4 - Muted blue (comments)
    inline static int FUNCTION = 0x50FA7B;           // #50FA7B - Green (functions)
    inline static int NUMBER = 0xBD93F9;             // #BD93F9 - Lavender (numbers)
    inline static int LINE_NUMBERS = 0x6C6783;       // #6C6783 - Muted purple-gray
    
    // UI Accents
    inline static int FOLDER_ICON = 0xFACC15;        // #FACC15 - Yellow for folders
    inline static int CODE_ICON = 0x60A5FA;          // #60A5FA - Blue for code files
    inline static int AI_FEATURE = 0x7E57C2;         // #7E57C2 - Purple for AI features
    inline static int ERROR = 0xFF3333;              // #FF3333 - Red for errors
    inline static int WARNING = 0xFACC15;            // #FACC15 - Yellow for warnings
    
    // Editor specific
    inline static int LINE_ACTIVE = 0x433755;        // #433755 - Active line highlight
    inline static int AI_SUGGESTION_BG = 0x433C7E;   // #433C7E with 95% opacity - AI suggestion background
};

// Specific colors for Scintilla in BGR format (note: Scintilla requires BGR instead of RGB)
struct ScintillaColors {
    // Syntax highlighting colors
    inline static int CODE_BG = 0x301818;            // #181830 - Darkest blue-purple (for code area) - BGR format
    inline static int BACKGROUND = 0x3F1E1E;         // #1E1E3F - Dark blue-purple background
    inline static int MARGIN_BG = 0x3F1E1E;          // #181830 - Use darker bg for margins (same as CODE_BG)
    inline static int DEFAULT_TEXT = 0xE0E0E0;       // #E0E0E0 - Light gray text
    inline static int KEYWORD = 0xC679FF;            // #FF79C6 - Pink (keywords)
    inline static int STRING = 0x8CFAF1;             // #F1FA8C - Yellow (strings)
    inline static int COMMENT = 0xA47262;            // #6272A4 - Muted blue (comments)
    inline static int FUNCTION = 0x7BFA50;           // #50FA7B - Green (functions)
    inline static int NUMBER = 0xF993BD;             // #BD93F9 - Lavender (numbers)
    inline static int OPERATOR = 0xC679FF;           // #FF79C6 - Pink (operators)
    inline static int LINE_NUMBERS = 0x83676C;       // #6C6783 - Muted purple-gray
    inline static int LINE_ACTIVE = 0x553743;        // #433755 - Active line highlight
    inline static int SELECTION_BG = 0xC2577E;       // #7E57C2 - Selection background
    inline static int SELECTION_FG = 0xFFFFFF;       // #FFFFFF - Selection foreground
    inline static int BRACE_MATCH = 0x7BFA50;        // #50FA7B - Matching brace highlight
    inline static int BRACE_MISMATCH = 0x3333FF;     // #FF3333 - Mismatched brace
};

}
}

#endif // MAGIA_THEME_H
