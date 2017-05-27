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
    class AweleGame : Game
    {
        private int[,] tab;

        public AweleGame()
            : base()
        {
            Name = "Awele";
            Description = "Most wisely known mancala game.\n." +
                "These are the major rules of Ivory Coast, as I leart them";
            WebURL = "http://askywhale.com/infiniteplay/";
            Version = 1;
            TreeDepth = 6;
            Approximation = 1.5f;

            InGame = false;
            tab = null;
        }

        public override void StartGame(int nbPlayers, int totalTimeMs)
        {
            base.StartGame(2, totalTimeMs);
            InGame = true;
            SetRandomNextPlayer();
            tab = new int[2, 6];
            for (int x = 0; x < 6; x++)
                for (int y = 0; y < 2; y++)
                    tab[y, x] = 4;
            for (int i = 0; i < 2; i++)
                Score[i] = 0;
        }

        public override void Paint()
        {
            drawImage("img.board.jpeg", 0, 0, 1, 1);
            double[] xx;
            double[] yy;
            xx = new double[48];
            yy = new double[48];
            if (tab != null)
            {
                for (int x = 0; x < 6; x++)
                    for (int y = 0; y < 2; y++)
                        for (int i = 0; i < tab[1 - y, x]; i++)
                        {
                            //some stupid stuffs to draw the seed somewhere near (x,y)
                            int i2 = 0;
                            bool ok = false;
                            do
                            {
                                //find a position
                                double angle = ((double)((x * 133 + y * 107 + i * 717 + i2 * 109) % 628)) / 100;
                                double dis = ((double)((x * 107 + y * 133 + i * 117 + i2 * 203) % 52)) / 1000;
                                xx[i] = .41 + (x - 2) * .152 * (1 + y * .04) + (dis * Math.Cos(angle));
                                yy[i] = .33 + y * .29 + (dis * Math.Sin(angle));
                                //verifie that another seed is not too near
                                ok = true;
                                if ((i > 0) && (i2 < i * 20)) //n*20 : heho! too much search!
                                {
                                    for (int j = 0; j < i; j++)
                                        if (Math.Abs(xx[i] - xx[j]) +
                                            Math.Abs(yy[i] - yy[j]) <
                                            .2 / (i + 5)) //too near
                                            ok = false;
                                }
                                i2++;
                            }
                            while (!ok);

                            //choose a picture of seed
                            int np = 1 + ((x * 103 + y * 109 + i * 107) % 6);
                            drawImage("img.piece" + np + ".png",
                                xx[i], yy[i], .04);
                        }
            }
            if (Score != null)
            {
                for (int n = 0; n < 2; n++)
                {
                    for (int i = 0; i < Score[n]; i++)
                        drawImage("img.piece" + (1 + (i % 6)) + ".png",
                            .07 + i * .02,
                            .82 - n * .8 + ((int)((1 + Math.Sin(i * 9 + n * 13)) * 91) % 11) / 100.0,
                            .04);
                    drawString("" + Score[n],
                        new Font(FontFamily.GenericSansSerif, 14),
                        new SolidBrush(Color.White), .005, .88 - .84 * n);
                }
            }
        }

        public override float[] Estimate()
        {
            int total = (int)(Score[0] + Score[1]);
            if (total == 0)
                return base.Estimate();
            float[] res = { (float)Score[0] / total, (float)Score[1] / total };
            return res;
        }

        public override int GetNbPossibleMoves()
        {
            int res = 0;
            for (int i = 0; i < 6; i++)
                if (tab[NextPlayer, i] > 0)
                    res++;
            return res;
        }

        public override void DoPossibleMove(int n)
        {
            int n2 = -1;
            for (int i = 0; i < 6; i++)
            {
                if (tab[NextPlayer, i] > 0)
                    n2++;
                if (n == n2)
                    take(i);
            }
        }

        public override Game GetCopy(Game gameToCopyInto)
        {
            if (gameToCopyInto == null)
                gameToCopyInto = new AweleGame();
            gameToCopyInto = base.GetCopy(gameToCopyInto);
            AweleGame ag = (AweleGame)gameToCopyInto;
            ag.tab = (int[,])tab.Clone();
            ag.Score = (int[])Score.Clone();
            return ag;
        }

        public override void ClickHere(double x, double y)
        {
            if (x > .03 && x < .97 &&
                y > .25 + (1 - NextPlayer) * .25 && y < .5 + (1 - NextPlayer) * .25)
                take((int)((x - .03) / ((.97 - .03) / 6)));
        }

        private void take(int p)
        {
            int p2, startP2;
            if (NextPlayer == 0)
                p2 = p + tab[NextPlayer, p];
            else
                p2 = 11 - p + tab[NextPlayer, p];
            p2 %= 12;
            startP2 = p2;
            bool taking = true;
            for (int i = 0; i < tab[NextPlayer, p]; i++, p2--)
            {
                if (p2 == -1)
                    p2 += 12;
                if (p2 == startP2 && i>0)
                    p2--;
                if (p2 == -1)
                    p2 += 12;
                int xx, yy;
                if (p2 >= 6)
                {
                    xx = 11 - p2;
                    yy = 1;
                }
                else
                {
                    xx = p2;
                    yy = 0;
                }
                tab[yy, xx]++;
                if ((tab[yy, xx] == 2 || tab[yy, xx] == 3) && taking)
                {
                    Score[NextPlayer] += tab[yy, xx];
                    tab[yy, xx] = 0;
                }
                else
                {
                    taking = false;
                }

            }
            tab[NextPlayer, p] = 0;
            NextPlayer = 1 - NextPlayer;
            if (GetNbPossibleMoves() == 0)
                InGame = false;
        }
    }
}
