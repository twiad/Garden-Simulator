/*
 *  GWEN
 *  Copyright (c) 2011 Facepunch Studios
 *  See license in Gwen.h
 */

#pragma once

#ifndef GWEN_RENDERERS_ALLEGRO_H
#define GWEN_RENDERERS_ALLEGRO_H

#include "Gwen/Gwen.h"
#include "Gwen/BaseRender.h"

#ifdef _WIN32
#   include <SDL.h>
#else
#   include <SDL2/SDL.h>
#endif


namespace Gwen
{
    namespace Renderer
    {
        class SDL2CTT;
        
        class SDL2 : public Gwen::Renderer::Base
        {
        public:
			SDL2(SDL_Renderer *renderer);
            SDL2(SDL_Window *window);
            ~SDL2();

            virtual void SetDrawColor(Gwen::Color color);

            virtual void DrawFilledRect(Gwen::Rect rect);

            virtual void LoadFont(Gwen::Font* pFont);
            virtual void FreeFont(Gwen::Font* pFont);
            virtual void RenderText(Gwen::Font* pFont, Gwen::Point pos,
                                    const Gwen::UnicodeString& text);
            virtual Gwen::Point MeasureText(Gwen::Font* pFont, const Gwen::UnicodeString& text);

            void StartClip();
            void EndClip();

            void DrawTexturedRect(Gwen::Texture* pTexture, Gwen::Rect pTargetRect,
                                  float u1 = 0.0f, float v1 = 0.0f,
                                  float u2 = 1.0f, float v2 = 1.0f);
            void        LoadTexture(Gwen::Texture* pTexture);
            void        FreeTexture(Gwen::Texture* pTexture);
            Gwen::Color PixelColour(Gwen::Texture* pTexture, unsigned int x, unsigned int y,
                                    const Gwen::Color& col_default);

            void DrawLinedRect(Gwen::Rect rect);

            bool BeginContext(Gwen::WindowProvider* pWindow);
            bool EndContext(Gwen::WindowProvider* pWindow);
            bool PresentContext(Gwen::WindowProvider* pWindow);

        protected:

            SDL_Renderer    *m_renderer;
            SDL_Color        m_color;
        };

		class SDL2Software : public Gwen::Renderer::Base
        {
        public:
			SDL2Software(SDL_Surface *surface);
            ~SDL2Software();

            virtual void SetDrawColor(Gwen::Color color);

            virtual void DrawFilledRect(Gwen::Rect rect);

            virtual void LoadFont(Gwen::Font* pFont);
            virtual void FreeFont(Gwen::Font* pFont);
            virtual void RenderText(Gwen::Font* pFont, Gwen::Point pos,
                                    const Gwen::UnicodeString& text);
            virtual Gwen::Point MeasureText(Gwen::Font* pFont, const Gwen::UnicodeString& text);

            void StartClip();
            void EndClip();

            void DrawTexturedRect(Gwen::Texture* pTexture, Gwen::Rect pTargetRect,
                                  float u1 = 0.0f, float v1 = 0.0f,
                                  float u2 = 1.0f, float v2 = 1.0f);
            void        LoadTexture(Gwen::Texture* pTexture);
            void        FreeTexture(Gwen::Texture* pTexture);
            Gwen::Color PixelColour(Gwen::Texture* pTexture, unsigned int x, unsigned int y,
                                    const Gwen::Color& col_default);

            void DrawLinedRect(Gwen::Rect rect);

            bool BeginContext(Gwen::WindowProvider* pWindow);
            bool EndContext(Gwen::WindowProvider* pWindow);
            bool PresentContext(Gwen::WindowProvider* pWindow);

        protected:
			SDL_Color        m_color;
			SDL_Surface		*m_surface;
            SDL_Renderer    *m_renderer;
        };
    }
}
#endif // ifndef GWEN_RENDERERS_ALLEGRO_H
