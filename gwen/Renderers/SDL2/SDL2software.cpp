
	// SDL2 Renderer for GWEN - http://www.libsdl.org/
	// Added by BQ.

	#include <Gwen/Gwen.h>
	#include <Gwen/BaseRender.h>
	#include <Gwen/Utility.h>
	#include <Gwen/Font.h>
	#include <Gwen/Texture.h>
	#include <Gwen/Renderers/SDL2.h>

	#ifdef _WIN32
	#   include <SDL_image.h>
	#   include <SDL_ttf.h>
	#else
	#   include <SDL2/SDL_image.h>
	#   include <SDL2/SDL_ttf.h>
	#endif

	namespace Gwen
	{
		namespace Renderer
		{

			SDL2Software::SDL2Software(SDL_Surface *surface)
			:   m_renderer(NULL), m_surface(surface)
			{
				m_renderer = SDL_CreateSoftwareRenderer(m_surface);
			}

			SDL2Software::~SDL2Software()
			{
				SDL_DestroyRenderer(m_renderer);
			}

			void SDL2Software::SetDrawColor(Gwen::Color color)
			{
				m_color.r = color.r;
				m_color.g = color.g;
				m_color.b = color.b;
				m_color.a = color.a;
            
				SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
			}

			void SDL2Software::LoadFont(Gwen::Font* font)
			{
				font->realsize = font->size*Scale();
				std::string fontFile(font->facename);

				if (fontFile.find(L".ttf") == std::string::npos)
					fontFile += L".ttf";

				TTF_Font *tfont = TTF_OpenFont(fontFile.c_str(), font->realsize);
				if (!tfont)
				{
					printf("Font load error: %s\n", TTF_GetError());
				}            
            
				font->data = tfont;
			}

			void SDL2Software::FreeFont(Gwen::Font* pFont)
			{
				if (pFont->data)
				{
					TTF_CloseFont(static_cast<TTF_Font*>(pFont->data));
					pFont->data = NULL;
				}
			}

			void SDL2Software::RenderText(Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString& text)
			{
				TTF_Font *tfont = static_cast<TTF_Font*>(pFont->data);
				Translate(pos.x, pos.y);
            
				SDL_Surface *surf = TTF_RenderUNICODE_Blended(tfont, text, m_color);

				const SDL_Rect src = {0,0,surf->w,surf->h};
				SDL_Rect dest = {pos.x,pos.y,surf->w,surf->h};
            
				SDL_BlitSurface(surf,&src,m_surface,&dest);
            
				SDL_FreeSurface(surf);
			}

			Gwen::Point SDL2Software::MeasureText(Gwen::Font* pFont, const Gwen::UnicodeString& text)
			{
				TTF_Font *tfont = static_cast<TTF_Font*>(pFont->data);

				// If the font doesn't exist, or the font size should be changed.
				if (!tfont || pFont->realsize != pFont->size*Scale())
				{
					FreeFont(pFont);
					LoadFont(pFont);
					tfont = static_cast<TTF_Font*>(pFont->data);
				}

				if (!tfont)
					return Gwen::Point(0, 0);

				int w,h;
				TTF_SizeUNICODE(tfont, text, &w,&h);
            
				return Point(w,h);
			}

			void SDL2Software::StartClip()
			{
				const Gwen::Rect &rect = ClipRegion();
				const SDL_Rect clip = { rect.x,rect.y, rect.w,rect.h };
				SDL_RenderSetClipRect(m_renderer, &clip);
			}

			void SDL2Software::EndClip()
			{
				SDL_RenderSetClipRect(m_renderer, NULL);
			}

			void SDL2Software::LoadTexture(Gwen::Texture* pTexture)
			{
				if (!pTexture)
					return;

				if (pTexture->data)
					FreeTexture(pTexture);
            
				SDL_Texture *tex = NULL;
            
				SDL_Surface *surf = IMG_Load(Gwen::Utility::UnicodeToString(pTexture->name).c_str());
				tex = SDL_CreateTextureFromSurface(m_renderer, surf);
				pTexture->surface = surf;
            

				if (tex)
				{
					int w, h;
					SDL_QueryTexture(tex, NULL, NULL, &w, &h);
                
					pTexture->data = tex;
					pTexture->width = w;
					pTexture->height = h;
					pTexture->failed = false;
				}
				else
				{
					pTexture->data = NULL;
					pTexture->failed = true;
				}
			}

			void SDL2Software::FreeTexture(Gwen::Texture* pTexture)
			{
				SDL_DestroyTexture(static_cast<SDL_Texture*>(pTexture->data));
				pTexture->data = NULL;
            
				if (pTexture->surface)
				{
					SDL_FreeSurface(static_cast<SDL_Surface*>(pTexture->surface));
					pTexture->surface = NULL;
					pTexture->readable = false;
				}
			}

			void SDL2Software::DrawTexturedRect(Gwen::Texture* pTexture, Gwen::Rect rect,
										float u1, float v1, float u2, float v2)
			{
				SDL_Texture *tex = static_cast<SDL_Texture*>(pTexture->data);

				if (!tex)
					return DrawMissingImage(rect);

				Translate(rect);
            
				const unsigned int w = pTexture->width;
				const unsigned int h = pTexture->height;
            
				const SDL_Rect source = { int(u1*w), int(v1*h), int((u2-u1)*w), int((v2-v1)*h) },
								 dest = { rect.x, rect.y, rect.w, rect.h };

				SDL_RenderCopy(m_renderer, tex, &source, &dest);
			}

			Gwen::Color SDL2Software::PixelColour(Gwen::Texture* pTexture, unsigned int x, unsigned int y,
										  const Gwen::Color& col_default)
			{
				SDL_Surface *surf = static_cast<SDL_Surface*>(pTexture->surface);

				if (!pTexture->readable || !surf)
					return col_default;
            
				if (SDL_MUSTLOCK(surf) != 0)
					SDL_LockSurface(surf);

				const char *mem = static_cast<char*>(surf->pixels) + y*surf->pitch + x*sizeof(Uint32);
				const Uint32 pix = *reinterpret_cast<const Uint32*>(mem);

				Gwen::Color col;
				SDL_GetRGBA(pix, surf->format, &col.r, &col.g, &col.b, &col.a);

				if (SDL_MUSTLOCK(surf) != 0)
					SDL_UnlockSurface(surf);

				return col;
			}

			void SDL2Software::DrawFilledRect(Gwen::Rect rect)
			{
				Translate(rect);
            
				const SDL_Rect srect = { rect.x, rect.y, rect.w, rect.h };
				SDL_RenderFillRect(m_renderer, &srect);
			}

			void SDL2Software::DrawLinedRect(Gwen::Rect rect)
			{
				Translate(rect);
            
				const SDL_Rect srect = { rect.x, rect.y, rect.w, rect.h };
				SDL_RenderDrawRect(m_renderer, &srect);
			}

			bool SDL2Software::BeginContext(Gwen::WindowProvider* )
			{
				SDL_RenderClear(m_renderer);
				SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
				return true;
			}

			bool SDL2Software::EndContext(Gwen::WindowProvider* pWindow)
			{
				return true;
			}

			bool SDL2Software::PresentContext(Gwen::WindowProvider* pWindow)
			{
				SDL_RenderPresent(m_renderer);
				return true;
			}

        
		} // Renderer
	} // Gwen
