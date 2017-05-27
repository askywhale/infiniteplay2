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
    class ChineseCheckersGame : Game
    {
        //fx
        private double[,] realPosX;
        private double[,] realPosY;

        //constants for one game
        private bool[,] hasHole;
        private int[] aims;
        private int[,] colorStartPointsX;
        private int[,] colorStartPointsY;

        //game
        private int[,] tab;
        private int selectedX, selectedY;
        bool selected, moved;
        private bool[,] everGoneTo;
        private int[,] possibleMoves; //dim. 2 : x1, y1, x2, y2

        public ChineseCheckersGame()
            : base()
        {
            Name = "Chinese Checkers";
            Description = "Chinese Checkers is a board game that can be played " +
                    "by 2,3,4 or 6 people. The object of one of the game is to place " +
                    "your pieces in the starting position of " +
                    "your front opponent, by moving them through jumps " +
                    "over other pieces.  The game does not " +
                    "actually originate from China, but was given " +
                    "that name in the US to make it sound more " +
                    "exotic !";
            WebURL = "http://askywhale.com/infiniteplay/";
            Version = 1;
            TreeDepth = 4;
            Approximation = 1.5f;

            InGame = false;
            tab = null;
            colorStartPointsX = colorStartPointsY = null;
            aims = null;
            hasHole = null;
            realPosX = realPosY = null;
            everGoneTo = null;
            possibleMoves = null;
        }

        public override int[] GetPossiblePlayers()
        {
            int[] res = { 2, 3, 4, 6 };
            return res;
        }

        public override void StartGame(int nbPlayers, int totalTimeMs)
        {
            base.StartGame(nbPlayers, totalTimeMs);
            tab = new int[13, 17];
            for (int x = 0; x < 13; x++)
                for (int y = 0; y < 17; y++)
                    tab[x, y] = -1;
            createHasHole();
            createAims();
            createStartPoints();
            createRealPos();
            CanPlay = new bool[nbPlayers];
            Score = new int[nbPlayers];
            for (int i = 0; i < nbPlayers; i++)
            {
                int color = getColor(i);
                for (int j = 0; j < 10; j++)
                    tab[colorStartPointsX[color, j],
                        colorStartPointsY[color, j]] = i;
                CanPlay[i] = false;
                Score[i] = 0;
            }
            InGame = true;
            SetRandomNextPlayer();
            everGoneTo = new bool[13, 17];
            changePlayer();
        }

        public override void Paint()
        {
            drawImage("img.board.jpg", 0, 0, 1, 1);
            if (hasHole != null && realPosX != null && realPosY != null)
                for (int y = 0; y < 17; y++)
                    for (int x = 0; x < 13; x++)
                        if (hasHole[x, y])
                            if (tab[x, y] == -1)
                                drawImage("img.hole.png",
                                    realPosX[x, y], realPosY[x, y], .03);
                            else
                                drawImage("img.piece" + tab[x, y] + ".png",
                                    realPosX[x, y], realPosY[x, y]-0.01, .03);
        }

        public override float[] Estimate()
        {
            float[] res = new float[NbPlayers];
            if (!InGame)
                return base.Estimate();
            for (int i = 0; i < NbPlayers; i++)
                res[i] = 0;
            for (int y = 0; y < 17; y++)
                for (int x = 0; x < 13; x++)
                    if (tab[x, y] > -1)
                    {
                        int aim = aims[tab[x, y]];
                        res[tab[x, y]] += getDist(x, y,
                            colorStartPointsX[aim, 0],
                            colorStartPointsY[aim, 0]);
                    }
            float max = 0;
            for (int i = 0; i < NbPlayers; i++)
                if (res[i] > max)
                    max = res[i];
            float total = 0;
            for (int i = 0; i < NbPlayers; i++)
            {
                res[i] = 1 + max - res[i];
                total += res[i];
            }
            if (total > 0)
                for (int i = 0; i < NbPlayers; i++)
                    res[i] /= (float)total;
            return res;
        }

        public override int GetNbPossibleMoves()
        {
            if (possibleMoves == null)
                createPossibleMoves();
            int res = possibleMoves.Length >> 2;// /4
            if (moved)
                res++;
            return res;
        }

        public override void DoPossibleMove(int n)
        {
            if (possibleMoves == null)
                createPossibleMoves();
            if (n == possibleMoves.Length >> 2)// /4 //end play
            {
                //Console.WriteLine(NextPlayer + " : Change player");
                changePlayer();
            }
            else
            {
                if (!selected)
                {
                    selected = true;
                    selectedX = possibleMoves[n, 0];
                    selectedY = possibleMoves[n, 1];
                }
                jumpHere(possibleMoves[n, 2], possibleMoves[n, 3]);
            }
        }

        public override Game GetCopy(Game gameToCopyInto)
        {
            if (gameToCopyInto == null)
                gameToCopyInto = new ChineseCheckersGame();
            gameToCopyInto = base.GetCopy(gameToCopyInto);
            ChineseCheckersGame ccg = (ChineseCheckersGame)gameToCopyInto;
            ccg.tab = (int[,])tab.Clone();
            ccg.hasHole = hasHole;
            ccg.aims = aims;
            ccg.colorStartPointsX = colorStartPointsX;
            ccg.colorStartPointsY = colorStartPointsY;
            ccg.selectedX = selectedX;
            ccg.selectedY = selectedY;
            ccg.selected = selected;
            ccg.moved = moved;
            ccg.everGoneTo = (bool[,])everGoneTo.Clone();
            if (possibleMoves != null)
                ccg.possibleMoves = (int[,])possibleMoves.Clone();
            return ccg;
        }

        public override void ClickHere(double x, double y)
        {
            bool didSomething = false;
            for (int x2 = 0; x2 < 13; x2++)
                for (int y2 = 0; y2 < 17; y2++)
                    if (x > realPosX[x2, y2] && x < realPosX[x2, y2] + .04 &&
                        y > realPosY[x2, y2] && y < realPosY[x2, y2] + .04 &&
                        hasHole[x2, y2])
                    {
                        if (!selected && CanPlay[tab[x2, y2]])
                        {
                            selected = true;
                            selectedX = x2;
                            selectedY = y2;
                            didSomething = true;
                        }
                        else if (canJumpHere(x2, y2))
                        {
                            jumpHere(x2, y2);
                            didSomething = true;
                        }
                    }
            if (!didSomething && moved)
            {
                changePlayer();
            }
        }

        private int getColor(int realPlayer)
        {
            switch (realPlayer)
            {
                case 0:
                    return 0;
                case 1:
                    switch (NbPlayers)
                    {
                        case 2:
                            return 3;
                        case 3:
                            return 2;
                        default:
                            return 1;
                    }
                case 2:
                    switch (NbPlayers)
                    {
                        case 4:
                            return 3;
                        default:
                            return 2;
                    }
                case 3:
                    switch (NbPlayers)
                    {
                        case 4:
                            return 5;
                        default:
                            return 3;
                    }
                case 4:
                    return 4;
                default: //5
                    return 5;
            }
        }

        private void createHasHole()
        {
            hasHole = new bool[13, 17];
            for (int y = 0; y < 17; y++)
                for (int x = 0; x < 13; x++)
                {
                    switch (y)
                    {
                        case 0:
                        case 16:
                            hasHole[x, y] = x == 6;
                            break;
                        case 1:
                        case 15:
                            hasHole[x, y] = x == 5 || x == 6;
                            break;
                        case 2:
                        case 14:
                            hasHole[x, y] = x >= 5 && x <= 7;
                            break;
                        case 3:
                        case 13:
                            hasHole[x, y] = x >= 4 && x <= 7;
                            break;
                        case 4:
                        case 12:
                            hasHole[x, y] = x >= 0 && x <= 12;
                            break;
                        case 5:
                        case 11:
                            hasHole[x, y] = x >= 0 && x <= 11;
                            break;
                        case 6:
                        case 10:
                            hasHole[x, y] = x >= 1 && x <= 11;
                            break;
                        case 7:
                        case 9:
                            hasHole[x, y] = x >= 1 && x <= 10;
                            break;
                        case 8:
                            hasHole[x, y] = x >= 2 && x <= 10;
                            break;
                    }
                }
        }

        private int getDist(int x1, int y1, int x2, int y2)
        {
            int dy = Math.Abs(y2 - y1);
            int devy = (int)(dy / 2);
            if (((x2 > x1) ^ ((y1 % 2) == 0)) && dy % 2 == 1)
                devy++;
            return dy + Math.Max(0, Math.Abs(x2 - x1) - devy);
        }

        private int getWon()
        {
            for (int i = 0; i < NbPlayers; i++)
                if (useAll(aims[i], i))
                    return i;
            return -1;
        }

        //player c use position of color c
        private bool useAll(int c, int p)
        {
            for (int i = 0; i < 10; i++)
                if (tab[colorStartPointsX[c, i], colorStartPointsY[c, i]] != p)
                    return false;
            return true;
        }

        private void createAims()
        {
            aims = new int[NbPlayers];
            if (NbPlayers == 2)
            {
                aims[0] = 3;
                aims[1] = 0;
            }
            else if (NbPlayers == 3)
            {
                aims[0] = 2;
                aims[1] = 4;
                aims[2] = 0;
            }
            else if (NbPlayers == 4)
            {
                aims[0] = 3;
                aims[1] = 4;
                aims[2] = 0;
                aims[3] = 1;
            }
            else
            {
                aims[0] = 3;
                aims[1] = 4;
                aims[2] = 5;
                aims[3] = 0;
                aims[4] = 1;
                aims[5] = 2;
            }
        }

        private bool canJumpSomewhere()
        {
            if (!selected)
                return false;
            for (int x = Math.Max(0, selectedX - 2);
                x <= Math.Min(12, selectedX + 2); x++)
                for (int y = Math.Max(0, selectedY - 2);
                    y <= Math.Min(16, selectedY + 2); y++)
                    if (canJumpHere(x, y))
                        return true;
            return false;
        }

        private bool canJumpHere(int x, int y)
        {
            if (tab[x, y] > -1 || !selected || !hasHole[x, y])
                return false;
            int dist = getDist(x, y, selectedX, selectedY);
            if (!moved && dist == 1)
                return true;
            if (dist == 2 && !everGoneTo[x, y])
            {
                if (selectedY == y && tab[(selectedX + x) >> 1, y] > -1)
                    return true;
                if (selectedX != x && Math.Abs(selectedY - y) == 2)
                {
                    int midX = (selectedX + x) >> 1;
                    if (y % 2 == 1)
                        midX++;
                    if (tab[midX, (selectedY + y) >> 1] > -1)
                        return true;
                }
            }
            return false;
        }

        private void jumpHere(int x, int y)
        {
            //if (!canJumpHere(x, y))
            //    Console.WriteLine("aberation:");
            int dist = getDist(x, y, selectedX, selectedY);
            tab[x, y] = tab[selectedX, selectedY];
            tab[selectedX, selectedY] = -1;

            int won = getWon();
            if (won > -1)
            {
                for (int i = 0; i < NbPlayers; i++)
                    Score[i] = 0;
                Score[won] = 1;
                InGame = false;
            }
            else
            {
                everGoneTo[selectedX, selectedY] = true;
                selectedX = x;
                selectedY = y;
                moved = true;
                possibleMoves = null;
                if (dist == 1)
                    changePlayer();
                else if (dist == 2 && !canJumpSomewhere())
                    changePlayer();
            }
        }

        private void changePlayer()
        {
            NextPlayer++;
            moved = false;
            selected = false;
            for (int y = 0; y < 17; y++)
                for (int x = 0; x < 13; x++)
                    everGoneTo[x, y] = false;
            possibleMoves = null;
        }

        private void createPossibleMoves()
        {
            int nbRes = 0;
            int[,] res = new int[10 * 12 + 1, 4];

            if (!selected)
            {
                selected = true;
                int np = NextPlayer;
                for (int xf = 0; xf < 13; xf++)
                    for (int yf = 0; yf < 17; yf++)
                        if (tab[xf, yf] == np)
                        {
                            selectedX = xf;
                            selectedY = yf;
                            int maxx = Math.Min(12, selectedX + 2);
                            int maxy = Math.Min(16, selectedY + 2);
                            for (int x = Math.Max(0, selectedX - 2);
                                x <= maxx; x++)
                                for (int y = Math.Max(0, selectedY - 2);
                                    y <= maxy; y++)
                                    if (canJumpHere(x, y))
                                    {
                                        res[nbRes, 0] = xf;
                                        res[nbRes, 1] = yf;
                                        res[nbRes, 2] = x;
                                        res[nbRes, 3] = y;
                                        nbRes++;
                                    }

                        }
                selected = false;
            }
            else
            {
                int maxx = Math.Min(12, selectedX + 2);
                int maxy = Math.Min(16, selectedY + 2);
                for (int x = Math.Max(0, selectedX - 2); x <= maxx; x++)
                    for (int y = Math.Max(0, selectedY - 2); y <= maxy; y++)
                        if (canJumpHere(x, y))
                        {
                            res[nbRes, 0] = selectedX;
                            res[nbRes, 1] = selectedY;
                            res[nbRes, 2] = x;
                            res[nbRes, 3] = y;
                            nbRes++;
                        }
            }
            possibleMoves = new int[nbRes, 4];
            for (int i = 0; i < nbRes; i++)
                for (int j = 0; j < 4; j++)
                    possibleMoves[i, j] = res[i, j];
        }

        private void createRealPos()
        {
            realPosX = new double[13, 17];
            realPosY = new double[13, 17];
            for (int y = 0; y < 17; y++)
                for (int x = 0; x < 13; x++)
                {
                    double usedX = x;
                    if (y % 2 == 1)
                        usedX += .5;
                    realPosX[x, y] = .2 + .62 * usedX / 13;
                    realPosY[x, y] = .07 + .76 * y / 17;
                }
        }

        private void createStartPoints()
        {
            int i, j, k, x, y;
            int[] basePointX = { 6, 0, 0, 6, 12, 12 };
            int[] basePointY = { 16, 12, 4, 0, 4, 12 };
            colorStartPointsX = new int[6, 10];
            colorStartPointsY = new int[6, 10];
            for (i = 0; i < 6; i++)
            {
                int n = 0;
                for (y = 0; y < 17; y++)
                    for (x = 0; x < 13; x++)
                        if (hasHole[x, y] && getDist(basePointX[i], basePointY[i], x, y) < 4)
                        {
                            colorStartPointsX[i, n] = x;
                            colorStartPointsY[i, n] = y;
                            n++;
                        }
                for (j = 0; j < 9; j++)
                    for (k = j + 1; k < 10; k++)
                        if (getDist(basePointX[i], basePointY[i],
                                colorStartPointsX[i, j], colorStartPointsY[i, j]) >
                            getDist(basePointX[i], basePointY[i],
                                colorStartPointsX[i, k], colorStartPointsY[i, k]))
                        {
                            int tmp = colorStartPointsX[i, j];
                            colorStartPointsX[i, j] = colorStartPointsX[i, k];
                            colorStartPointsX[i, k] = tmp;
                            tmp = colorStartPointsY[i, j];
                            colorStartPointsY[i, j] = colorStartPointsY[i, k];
                            colorStartPointsY[i, k] = tmp;
                        }
            }
        }
    }

}
