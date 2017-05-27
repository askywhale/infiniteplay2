/*
InfinitePlay 2
Copyright (C) 2017, askywhale

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.
*/
using System;
using System.Drawing;

namespace Askywhale.InfinitePlay
{
    class FiveColorsGame : Game
    {
        private int[][] tab;
        private int[] pos;
        private int[][] taken;

        public FiveColorsGame()
            : base()
        {
            Name = "5 colors";
            Description = "As I discovered this game in a old game box, " +
                    "I could not find anything about it anywhere. " +
                    "But the rules are simple enough to play without " +
                    "any rulebook.";
            WebURL = "http://askywhale.com/infiniteplay/";
            Version = 1;
            TreeDepth = 4;
            Approximation = 1.5f;
            
            InGame = false;
            tab = null;
            pos = null;
            taken = null;
        }

        public override void StartGame(int nbPlayers, int totalTimeMs)
        {
            base.StartGame(2, totalTimeMs);
            InGame = true;
            NextPlayer = random.Next(2);
            int[] nb = new int[5];

            for (int i = 0; i < 5; i++)
                nb[i] = 0;
            tab = new int[5][];
            for (int x = 0; x < 5; x++)
            {
                tab[x] = new int[5];
                for (int y = 0; y < 5; y++)
                {
                    int n = 0;
                    do
                        n = random.Next(5);
                    while (nb[n] > n + 2);
                    tab[x][y] = n;
                    nb[n]++;
                }
            }
            taken = new int[2][];
            pos = new int[2];
            for (int i = 0; i < 2; i++)
            {
                pos[i] = random.Next(5);
                taken[i] = new int[5];
                for (int n = 0; n < 5; n++)
                    taken[i][n] = 0;
            }
        }

        public override void Paint()
        {
            drawImage("img.board.jpg", 0, 0, 1, 1);
            if (tab != null)
            {
                for (int x = 0; x < 5; x++)
                    for (int y = 0; y < 5; y++)
                        if (tab[x][y] > -1)
                            drawImage("img." + tab[x][y] + ".png",
                                .225 + .115 * x,
                                .065 + .164 * y,
                                .1);
            }
            if (taken != null)
            {
                for (int n = 0; n < 2; n++)
                    for (int i = 0; i < 5; i++)
                        for (int j = 0; j < taken[n][i]; j++)
                            drawImage("img." + i + ".png",
                                .05 + n * .9 + (1 - 2 * n) * (i * .03) - .042,
                                .8 - j * .03,
                                .1);

            }
            if (InGame)
            {
                string[] ims = { "miniblanc2", "miniblanc2" };
                ims[NextPlayer] = "miniblanc";
                drawImage("img." + ims[0] + ".png", .24 + .115 * pos[0], .85, .07);
                drawImage("img." + ims[1] + ".png", .78, .09 + .164 * pos[1], .07);
            }
            if(Score != null)
                for (int i = 0; i < 2; i++)
                    drawString("" + Score[i], 
                        new Font(FontFamily.GenericSansSerif, 14),
                        new SolidBrush(Color.White), .1 + .78 * i, .92);
        }

        public override float[] Estimate()
        {
            int total = (int)(Score[0] + Score[1]);
            if(total==0) 
                return base.Estimate();
            float[] res = { (float)Score[0] / total, (float)Score[1] / total };
            return res;
        }

        public override int GetNbPossibleMoves() 
        { 
            int res = 0;
            for (int i = 0; i < 5; i++)
                if (mayPlay(i))
                    res++;
            return res;        
        }

        public override void DoPossibleMove(int n) 
        {
            int n2 = -1;
            for (int i = 0; i < 5; i++)
            {
                if (mayPlay(i))
                    n2++;
                if (n == n2)
                    play(i);
            }               
        }

        public override Game GetCopy(Game gameToCopyInto)
        {
            if (gameToCopyInto == null)
                gameToCopyInto = new FiveColorsGame();
            gameToCopyInto = base.GetCopy(gameToCopyInto);
            FiveColorsGame fcg = (FiveColorsGame)gameToCopyInto;
            fcg.tab = (int[][])tab.Clone();
            for (int x = 0; x < 5; x++)
                fcg.tab[x] = (int[])fcg.tab[x].Clone();
            fcg.pos = (int[])pos.Clone();
            fcg.taken = (int[][])taken.Clone();
            for (int i = 0; i < 2; i++)
                fcg.taken[i] = (int[])fcg.taken[i].Clone();
            return fcg;
        }

        public override void ClickHere(double x, double y)
        {
            if (NextPlayer == 0)
            {
                double baseLine = .22;
                if (x > baseLine && x < baseLine + .115 * 5)
                    play((int)((x - baseLine) / .115));
            }
            else
            {
                double baseLine = .07;
                if (y > baseLine && y < baseLine + .15 * 5)
                    play((int)((y - baseLine) / .15));
            }
        }

        private void play(int p)
        {
            if (!mayPlay(p))
                return;
            pos[NextPlayer] = p;
            taken[NextPlayer][tab[pos[0]][pos[1]]]++;
            tab[pos[0]][pos[1]] = -1;

            int r = 0;
            for (int i = 0; i < 5; i++)
                r += taken[NextPlayer][i] * taken[NextPlayer][i];
            Score[NextPlayer] = r;

            NextPlayer = 1 - NextPlayer;
            bool newInGame = false;
            for (int i = 0; i < 5; i++)
                newInGame |= mayPlay(i);
            InGame = newInGame;
            //invalidate();
        }

        private bool mayPlay(int p)
        {
            if (!InGame)
                return false;
            if (p == pos[NextPlayer])
                return false;
            if (NextPlayer == 0 && tab[p][pos[1]] == -1)
                return false;
            if (NextPlayer == 1 && tab[pos[0]][p] == -1)
                return false;
            return true;
        }
    }
}
