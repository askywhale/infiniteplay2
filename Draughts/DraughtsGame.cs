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
    class DraughtsGame : Game
    {
        private int[][] tab;
        private int selectedX, selectedY;
        private int mustTakeWithX, mustTakeWithY;
        private bool selected, shouldTake, mustTakeWithOne;
        private int[][] possibleMoves;

        public DraughtsGame()
            : base()
        {
            Name = "Draughts";
            Description = "";
            WebURL = "http://askywhale.com/infiniteplay/";
            Version = 1;
            TreeDepth = 5;
            Approximation = 1.5f;

            InGame = false;
            tab = null;
            possibleMoves = null;
        }

        public override void StartGame(int nbPlayers, int totalTimeMs)
        {
            base.StartGame(2, totalTimeMs);
            InGame = true;
            NextPlayer = 0;
            selected = false;
            shouldTake = false;
            mustTakeWithOne = false;

            tab = new int[10][];
            for (int x = 0; x < 10; x++)
            {
                tab[x] = new int[10];
                for (int y = 0; y < 10; y++)
                {
                    if ((x + y) % 2 == 1)
                    {
                        if (y >= 6)
                            tab[x][y] = 0;
                        else if (y <= 3)
                            tab[x][y] = 1;
                        else
                            tab[x][y] = -1;
                    }
                    else
                        tab[x][y] = -1;

                }
            }
        }

        public override void Paint()
        {
            drawImage("img.boardd.jpg", 0, 0, 1, 1);
            if (tab != null)
            {
                for (int x = 0; x < 10; x++)
                    for (int y = 0; y < 10; y++)
                        if (tab[x][y] > -1)
                        {
                            double px = .19 + .064 * x - 0.005 + 0.01 * (((x * 5 + y * 8 + x * y) % 10) / 10.0);
                            double py = .06 + .07 * Math.Pow(y, 1.1) - 0.005 + 0.01 * (((x * 7 + y * 3 + x * y * 5) % 10) / 10.0);
                            px = .5 + (px - .5) * (1 + (double)y / 60);
                            drawImage("img.d" + tab[x][y] + ".png",
                                px, py, .045 + (double)y * 0.0002);
                        }
            }
        }

        public override float[] Estimate()
        {
            int[] usedScore;
            if (!InGame)
            {
                usedScore = Score;
            }
            else
            {
                usedScore = new int[] { 0, 0 };
                for (int x = 0; x < 10; x++)
                    for (int y = 0; y < 10; y++)
                        switch (tab[x][y])
                        {
                            case 0: usedScore[0]++; break;
                            case 1: usedScore[1]++; break;
                            case 2: usedScore[0]+=4; break;
                            case 3: usedScore[1]+=4; break;
                        }

            }
            int total = usedScore[0] + usedScore[1];
            if (total == 0)
                return base.Estimate();
            float[] res = { (float)usedScore[0] / total, 
                (float)usedScore[1] / total };
            return res;
        }

        public override int GetNbPossibleMoves()
        {
            return getPossibleMoves().Length;
        }

        public override void DoPossibleMove(int n)
        {
            int[][] moves = getPossibleMoves();
            int[] move = moves[n];
            play(move[0], move[1], move[2], move[3]);
        }

        private int[][] getPossibleMoves()
        {
            if (possibleMoves != null)
                return possibleMoves;
            int[][] res = new int[400][];
            int nbRes = 0;
            for (int x1 = 0; x1 < 10; x1++)
                for (int y1 = 0; y1 < 10; y1++)
                    if (tab[x1][y1] > -1 && tab[x1][y1] % 2 == NextPlayer)
                    {
                        int lim = 2;
                        if (tab[x1][y1] > 1)
                            lim = 9;
                        for (int x = x1 - lim, y = y1 - lim; x <= x1 + lim; x++, y++)
                            if (x >= 0 && x < 10 && y >= 0 && y < 10 &&
                                mayPlay(x1, y1, x, y))
                            {
                                res[nbRes] = new int[] { x1, y1, x, y };
                                nbRes++;
                            }
                        for (int x = x1 - lim, y = y1 + lim; x <= x1 + lim; x++, y--)
                            if (x >= 0 && x < 10 && y >= 0 && y < 10 &&
                                    mayPlay(x1, y1, x, y))
                            {
                                res[nbRes] = new int[] { x1, y1, x, y };
                                nbRes++;
                            }
                    }
            int[][] realRes = new int[nbRes][];
            for (int i = 0; i < nbRes; i++)
                realRes[i] = res[i];
            possibleMoves = realRes;
            return realRes;
        }


        public override Game GetCopy(Game gameToCopyInto)
        {
            if (gameToCopyInto == null)
                gameToCopyInto = new DraughtsGame();
            gameToCopyInto = base.GetCopy(gameToCopyInto);
            DraughtsGame dg = (DraughtsGame)gameToCopyInto;
            dg.tab = (int[][])tab.Clone();
            for (int x = 0; x < 10; x++)
                dg.tab[x] = (int[])tab[x].Clone();
            if (possibleMoves != null)
            {
                dg.possibleMoves = (int[][])possibleMoves.Clone();
                for (int i = 0; i < possibleMoves.Length; i++)
                    dg.possibleMoves[i] = (int[])possibleMoves[i].Clone();
            }
            dg.shouldTake = shouldTake;
            dg.mustTakeWithX = mustTakeWithX;
            dg.mustTakeWithY = mustTakeWithY;
            dg.mustTakeWithOne = mustTakeWithOne;
            return dg;
        }

        public override void ClickHere(double x, double y)
        {
            if (!InGame)
                return;
            int xx = (int)Math.Floor(5 + (x - .5) * (16 - y * 2.5));
            int yy = (int)Math.Floor(Math.Pow(y - .06, .9) * 11.5);
            if (xx < 0 || xx > 9 || yy < 0 || yy > 9 || (xx + yy) % 2 == 0)
                return;
            bool maySelect = !selected;
            if (selected)
            {
                selected = false;
                int oldNextPlayer = NextPlayer;
                if (mayPlay(selectedX, selectedY, xx, yy))
                    play(selectedX, selectedY, xx, yy);
                maySelect = oldNextPlayer == NextPlayer;
            }

            if (maySelect)
            {
                if (tab[xx][yy] == NextPlayer || tab[xx][yy] == NextPlayer + 2)
                {
                    selectedX = xx;
                    selectedY = yy;
                    selected = true;
                }
            }
        }

        //not enough strict : positions for draughts after take
        private bool mayPlay(int x1, int y1, int x2, int y2)
        {
            if (tab[x1][y1] == -1 || (tab[x1][y1] % 2) != NextPlayer)
                return false;
            if (mustTakeWithOne &&
                    (x1 != mustTakeWithX || y1 != mustTakeWithY))
                return false;
            if (x1 == x2 || y1 == y2)
                return false;
            if ((x2 + y2) % 2 == 0 || tab[x2][y2] > -1)
                return false;
            bool res = false;
            if (Math.Abs(x1 - x2) == 1 && Math.Abs(y1 - y2) == 1 &&
                    (NextPlayer == 0 ^ y2 > y1) && !shouldTake)
                res = true;
            if (Math.Abs(x1 - x2) == 2 && Math.Abs(y1 - y2) == 2 &&
                    (tab[(x1 + x2) / 2][(y1 + y2) / 2] % 2) > -1 &&
                    (tab[(x1 + x2) / 2][(y1 + y2) / 2] % 2) % 2 != NextPlayer)
                res = true;
            if (Math.Abs(x1 - x2) >= 2 && Math.Abs(x1 - x2) == Math.Abs(y1 - y2) &&
                tab[x1][y1] > 1)
            {
                int nbOpp = 0;
                int xd = Math.Abs(x1 - x2) / (x2 - x1);
                int yd = Math.Abs(y1 - y2) / (y2 - y1);
                for (int xx = x1 + xd, yy = y1 + yd; xx != x2; xx += xd, yy += yd)
                {
                    if (tab[xx][yy] > -1)
                        if (tab[xx][yy] % 2 == NextPlayer)
                            return false;
                        else
                            nbOpp++;
                }
                if (nbOpp == 0 && shouldTake)
                    return false;
                else
                    return nbOpp < 2;
            }
            if (!res)
                return false;
            return res;
        }

        private bool mayTakeFrom(int x1, int y1)
        {
            if (tab[x1][y1] == -1 || (tab[x1][y1] % 2) != NextPlayer)
                return false;
            int lim = 2;
            if (tab[x1][y1] > 1)
                lim = 9;
            for (int x = x1 - lim, y = y1 - lim; x <= x1 + lim; x++, y++)
                if (x >= 0 && x < 10 && y >= 0 && y < 10 &&
                        Math.Abs(x - x1) >= 2 && mayPlay(x1, y1, x, y))
                {
                    int xd = Math.Abs(x1 - x) / (x1 - x);
                    for (int xx = x + xd, yy = y + xd; xx != x1; xx += xd, yy += xd)
                        if (tab[xx][yy] > -1 && tab[xx][yy] % 2 != NextPlayer)
                            return true;
                }
            for (int x = x1 - lim, y = y1 + lim; x <= x1 + lim; x++, y--)
                if (x >= 0 && x < 10 && y >= 0 && y < 10 &&
                        Math.Abs(x - x1) >= 2 && mayPlay(x1, y1, x, y))
                {
                    int xd = Math.Abs(x1 - x) / (x1 - x);
                    for (int xx = x + xd, yy = y - xd; xx != x1; xx += xd, yy -= xd)
                        if (tab[xx][yy] > -1 && tab[xx][yy] % 2 != NextPlayer)
                            return true;
                }

            return false;
        }

        private void play(int x1, int y1, int x2, int y2)
        {
            tab[x2][y2] = tab[x1][y1];
            tab[x1][y1] = -1;
            if (y2 == 0 && NextPlayer == 0)
                tab[x2][y2] = 2;
            if (y2 == 9 && NextPlayer == 1)
                tab[x2][y2] = 3;
            bool doChangePlayer = true;
            if (Math.Abs(x2 - x1) >= 2)
            {
                bool taken = false;
                if (Math.Abs(x2 - x1) == 2)
                {
                    tab[(x1 + x2) / 2][(y1 + y2) / 2] = -1;
                    taken = true;
                }
                else
                {
                    int xd = Math.Abs(x1 - x2) / (x2 - x1);
                    int yd = Math.Abs(y1 - y2) / (y2 - y1);
                    for (int xx = x1 + xd, yy = y1 + yd; xx != x2; xx += xd, yy += yd)
                    {
                        if (tab[xx][yy] > -1)
                        {
                            tab[xx][yy] = -1;
                            taken = true;
                        }
                    }
                }
                if (taken)
                {
                    InGame = false;
                    for (int x = 0; x < 10; x++)
                        for (int y = 0; y < 10; y++)
                            if (tab[x][y] > -1 && (tab[x][y] % 2) != NextPlayer)
                                InGame = true;
                    if (!InGame)
                    {
                        Score[NextPlayer] = 1;
                        Score[1 - NextPlayer] = 0;
                    }
                    else
                    {
                        doChangePlayer = !mayTakeFrom(x2, y2);
                        if (!doChangePlayer)
                        {
                            mustTakeWithX = x2;
                            mustTakeWithY = y2;
                            mustTakeWithOne = true;
                        }
                        else
                            mustTakeWithOne = false;
                    }
                }
            }
            if (doChangePlayer)
            {
                NextPlayer = 1 - NextPlayer;
                mustTakeWithOne = false;
                shouldTake = false;
                for (int x = 0; x < 10; x++)
                    for (int y = 0; y < 10; y++)
                        if (tab[x][y] > -1 && tab[x][y] % 2 == NextPlayer
                                && mayTakeFrom(x, y))
                            shouldTake = true;
            }
            possibleMoves = null;
        }
    }
}
