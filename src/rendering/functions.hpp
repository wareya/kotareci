
void renderTextureInternal( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, int w, int h, bool flipx );

void draw_sprite(SDL_Texture * sprite, float x, float y, float angle = 0, float xscale = 1, float yscale = 1, float alpha = 1)
{
	int w, h;
	SDL_QueryTexture( tex, NULL, NULL, &w, &h );
	w = round(w*scale);
	h = round(h*scale);
    renderTextureInternal(sprite, Sys::renderer, roundf(x), roundf(y), 
}
draw_line(


SDL_RenderDrawLine(Sys::Renderer, bullet->position->x-x, bullet->position->y-y, bullet->position->x-dx-x, bullet->position->y-dy-y);
