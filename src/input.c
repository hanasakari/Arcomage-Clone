#include <SDL.h>
#include "graphics.h"
#include "adapter.h"
//#include "sound.h"

SDL_Event event;
int bRefreshNeeded=0; ///< True if we need to refresh the screen. Used in the input loop.

void WaitForKey(int sym)
{
	if (!sym)
	{
		do
			{
				SDL_PollEvent(&event);
				SDL_Delay(CPUWAIT);
			} while (event.type!=SDL_KEYUP);
	} else {
		do
		{
			SDL_PollEvent(&event);
			SDL_Delay(CPUWAIT);
		} while (event.type!=SDL_KEYDOWN || event.key.keysym.sym!=sym);
	}
}

void WaitForInput()
{
	/*
	do
	{
		SDL_PollEvent(&event);
		SDL_Delay(CPUWAIT);
	} while (!((event.type==SDL_KEYDOWN)||((event.type==SDL_MOUSEBUTTONDOWN)&&(event.button.button==SDL_BUTTON_LEFT))));

	if (event.type!=SDL_KEYDOWN)
	{
		do
		{
			SDL_PollEvent(&event);
			SDL_Delay(CPUWAIT);
		} while (event.type!=SDL_MOUSEBUTTONUP||event.button.button!=SDL_BUTTON_LEFT);
	}
	*/

	SDL_PumpEvents();
	do {
		SDL_PollEvent(&event);
		SDL_Delay(CPUWAIT);
	} while (!((event.type==SDL_KEYUP)||((event.type==SDL_MOUSEBUTTONUP)&&(event.button.button==SDL_BUTTON_LEFT))));
	SDL_PumpEvents();
}

/**
 * The main loop in the game.
 *
 * Includes the event loop, victory/loss handling, AI and network support.
 *
 * Authors: STiCK, GreatEmerald.
 */
void DoGame()
{
    int i, n;
    int crd,netcard;
    char bDiscarded=0, bAllowedToPlay=0;

    while (!IsVictorious(0) && !IsVictorious(1))
    {
        DrawScene();
        UpdateScreen();

        while (SDL_PollEvent(&event));//GE: Delete all events from the event queue before our turn.

        if (GetIsAI(Turn))
        {
            SDL_Delay(500);
            AIPlay();
        } /*else //GEm: TODO Netplay
        if (turn==netplayer)
        {
            if (NetRemPlay(&i,&discrd) && CanPlayCard(i,discrd))
			{
				PlayCardAnimation(i, discrd);
				PlayCard(i,discrd);
			}
            else {
                DrawDialog(DLGERROR,"Server dropped connection ...");
                WaitForInput();
                return;
            }
        } */
        else
        {
            while (!SDL_PollEvent(&event))
                SDL_Delay(0); //GEm: HACK
            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE) //GEm: Return if Esc is pressed.
                return;
            /*if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_b) //GE: Keeping as "down" since it's urgent ;)
                Boss();*/ //GEm: TODO boss screen
            /*if ( event.type == SDL_MOUSEMOTION && InRect(event.motion.x, event.motion.y,   8,342,  8+94,468) ) //GE: Support for highlighting cards, to be done: card tooltips.
            {
                Blit(SCREEN, BUFFER);
                UpdateScreen();
                bRefreshNeeded=1;
            }
            else if(bRefreshNeeded)
            {
                RedrawScreen(turn, Player);
                bRefreshNeeded=0;
            }*/ //GEm: TODO: Card highlighting

            if (event.type != SDL_MOUSEBUTTONUP || event.button.button > 3)
            {
                SDL_Delay(0); //GEm: HACK
                continue;
            }
            bDiscarded = (event.button.button == 2) || (event.button.button == 3);
            for (i=0; i<GetConfig(CardsInHand); i++)
            {
                if (FInRect((float)event.button.x/GetConfig(ResolutionX), (float)event.button.y/GetConfig(ResolutionY),
                    CardLocations[Turn][i].X, CardLocations[Turn][i].Y,
                    CardLocations[Turn][i].X+94/800.0, CardLocations[Turn][i].Y+128/600.0)
                    /*&&  GetCanPlayCard(Turn, i, bDiscarded)*/)
                {
                    crd=i;
                    bAllowedToPlay = 1; //GEm: This only checks for special conditions, not resources!
                    break;
                }
            }
            //netcard = Player[turn].Hand[crd];//GEm: TODO: Netplay
            if (bAllowedToPlay)
            {
                ExecutePlayCard(crd, bDiscarded);
                bAllowedToPlay = 0;
            }
            
            /*if (netplayer!=-1)
                NetLocPlay(crd,discrd,netcard);*/ //GEm: TODO: Netplay
        }
        SDL_Delay(CPUWAIT);
    }
    
    //printf("DoGame(): Info: Game ended: Red gets %d, blue gets %d!\n", IsVictorious(0), IsVictorious(1));
    DrawScene();
    if (IsVictorious(0) && IsVictorious(1))
    {
        DrawDialog(DLGWINNER,"Draw!");
        //Sound_Play(VICTORY); //GEm: TODO: Sound
    }
    else
    {
        if (GetIsAI(1))              // 1 local Player //GEm: TODO: more than 2 players
        {
            //i=aiplayer;if (i==-1) i=netplayer;i=!i; //GEm: TODO: Networking support
            if (IsVictorious(0))
            {
                if (GetResource(0, RT_Tower) >= GetConfig(TowerVictory))
                    DrawDialog(DLGWINNER, "You win by a\ntower building victory!");
                else if (GetResource(1, RT_Tower) <= 0)
                    DrawDialog(DLGWINNER, "You win by a tower\ndestruction victory!");
                else DrawDialog(DLGWINNER, "You win by a\nresource victory!");
                //Sound_Play(VICTORY);
            }
            else
            {
                if (GetResource(1, RT_Tower) >= GetConfig(TowerVictory))
                    DrawDialog(DLGLOOSER, "You lose by a\ntower building defeat!");
                else if (GetResource(0, RT_Tower) <= 0)
                    DrawDialog(DLGLOOSER, "You lose by a\ntower destruction defeat!");
                else DrawDialog(DLGLOOSER, "You lose by a\nresource defeat!");
                //Sound_Play(DEFEAT);
            }
        } else {                                         // 2 local Players
            if (IsVictorious(0))
                DrawDialog(DLGWINNER,"Winner is\n%s !", GetPlayerName(0));
            else
                DrawDialog(DLGWINNER,"Winner is\n%s !", GetPlayerName(1));
            //Sound_Play(VICTORY);
        }
    }
    UpdateScreen();
    SDL_Delay(1000);
    while (SDL_PollEvent(&event));
    WaitForInput();
}
