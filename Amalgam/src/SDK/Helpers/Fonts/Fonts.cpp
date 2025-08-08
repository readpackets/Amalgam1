#include "Fonts.h"

void CFonts::Reload(float flDPI)
{
    m_mFonts.clear();
    
    m_mFonts[FONT_NITRO] = {
        "Tahoma",
        int(12.f * flDPI),
        0,
        0
    };

    m_mFonts[FONT_ESP] = m_mFonts[FONT_NITRO];
    m_mFonts[FONT_INDICATORS] = m_mFonts[FONT_NITRO];

    for (auto& [eFont, fFont] : m_mFonts)
    {
        fFont.m_dwFont = I::MatSystemSurface->CreateFont();
        if (fFont.m_dwFont)
        {
            I::MatSystemSurface->SetFontGlyphSet(
                fFont.m_dwFont,
                fFont.m_szName,
                fFont.m_nTall,
                fFont.m_nWeight,
                0, 0,
                fFont.m_nFlags
            );
        }
    }
}

const Font_t& CFonts::GetFont(EFonts eFont)
{
    return m_mFonts.at(eFont);
}
