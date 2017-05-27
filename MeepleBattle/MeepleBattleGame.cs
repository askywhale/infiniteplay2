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
    class MeepleBattleGame : Game
    {
        // true : pass when blocked, false : lose when blocked
        private const bool REGULAR_PASS = true; 

        private int[,] colorCodes = { 
                                    { 0, 0, 0 }, 
                                    { 40, 40, 160 },
                                    { 40, 160, 40 },
                                    { 40, 160, 160 },
                                    { 160, 40, 40 },                                    
                                    { 160, 40, 160 },
                                    { 160, 160, 40 },
                                    { 160, 160, 160 },
                                    };
        private int[,] colors;
        private int[, ,] positions;
        private Bitmap boardBitmap;
        private int nextColor;
        private int nbNoMove;
        private bool showPossibleMoves;
        private bool[,] allowed;

        public MeepleBattleGame()
            : base()
        {
            Name = "Meeple Battle";
            Description = "Move a meeple on the board : the destination square color is the next meeple of the opponent.\n" +
                "You win on the last row, but you lose or pass if you have no possible move.";
            WebURL = "http://askywhale.com/infiniteplay/";
            Version = 1;
            TreeDepth = 4;
            Approximation = 1.5f;

            InGame = false;
            createImageIfNeeded("img.board.jpeg");
            boardBitmap = new Bitmap(images["img.board.jpeg"]);

        }

        public override void StartGame(int nbPlayers, int totalTimeMs)
        {
            base.StartGame(nbPlayers, totalTimeMs);
            colors = new int[8, 8];
            int nbTry = 0;
            do
            {
                for (int x = 0; x < 8; x++)
                    for (int y = 0; y < 8; y++)
                        colors[x, y] = -1;            
                for (int n = 0; n < 8; n++)
                {
                    for (int y = 0; y < 4; y++)
                    {
                        int x = 0;
                        do
                        {
                            x = random.Next(8);
                        } while (colors[x, y] > -1);
                        colors[x, y] = n;
                        colors[7 - x, 7 - y] = n;
                    }                     
                }
                nbTry++;
            } while (getFieldQuality() < 0.6 - nbTry * 0.00001);

            boardBitmap = new Bitmap(images["img.board.jpeg"]);
            for (int x = 0; x < 1280; x++)
            {
                for (int y = 0; y < 876; y++)
                {
                    Color c;
                    if (x >= 1280 / 2 - 800 / 2 && x < 1280 / 2 + 800 / 2 &&
                            y >= 876 / 2 - 800 / 2 && y < 876 / 2 + 800 / 2)
                    {
                        int xp = (x - (1280 / 2 - 800 / 2));
                        int yp = (y - (876 / 2 - 800 / 2));
                        int value = 20 + random.Next(5);
                        /*if (yp < 10 || yp>90 || xp < 10 || xp > 90)
                            value -= 20; */
                        value += 10;
                        value -= (((xp / 25) % 7) + ((yp / 25) % 5) + ((xp / 25) * (yp / 25) % 9));
                        int color = getColorFromRealPos(x, y);
                        c = Color.FromArgb(value + colorCodes[color, 0],
                            value + colorCodes[color, 1],
                            value + colorCodes[color, 2]);
                        boardBitmap.SetPixel(x, y, c);
                    }
                }
            }
            positions = new int[2, 8, 2];
            for (int p = 0; p < 2; p++)
            {
                for (int i = 0; i < 8; i++)
                {
                    if (p == 0)
                    {
                        positions[p, i, 0] = i;
                        positions[p, i, 1] = 7;
                    }
                    else
                    {
                        positions[p, i, 0] = 7 - i;
                        positions[p, i, 1] = 0;
                    }
                }
            }

            SetRandomNextPlayer();
            nextColor = random.Next(8);
            allowed = new bool[8, 8];
            InGame = true;
            updateAllowedMoves();
        }

        public override void Paint()
        {
            if (boardBitmap != null)
                drawImage(boardBitmap, 0, 0, 1, 1);
            if (InGame && allowed != null && showPossibleMoves)
            {
                for (int x = 0; x < 8; x++)
                    for (int y = 0; y < 8; y++)
                        if (allowed[x, y])
                            drawImage("img.possible.png",
                                (x * 100 + (1280 / 2 - 800 / 2)) / 1280.0,
                                (y * 100 + (876 / 2 - 800 / 2)) / 876.0,
                            100 / 1280.0, 100 / 876.0);
            }
            if (positions != null)
                for (int p = 0; p < 2; p++)
                    for (int i = 0; i < 8; i++)
                        drawImage("img." + p + "-" + i + ".png",
                            (positions[p, i, 0] * 100 + (1280 / 2 - 800 / 2)) / 1280.0,
                            (positions[p, i, 1] * 100 + (876 / 2 - 800 / 2)) / 876.0,
                            100 / 1280.0, 100 / 876.0);
            if (positions != null && InGame)
                drawImage("img." + NextPlayer + "-s.png",
                            (positions[NextPlayer, nextColor, 0] * 100 + (1280 / 2 - 800 / 2)) / 1280.0,
                            (positions[NextPlayer, nextColor, 1] * 100 + (876 / 2 - 800 / 2)) / 876.0,
                            100 / 1280.0, 100 / 876.0);
        }

        public override float[] Estimate()
        {
            float[] res = { 1 + (float)Score[0], 1 + (float)Score[1] };
            if (res[0] + res[1] == 2)
            {
                for (int p = 0; p < 2; p++)
                    for (int i = 0; i < 8; i++)
                    {
                        int ny = positions[p, i, 1] - 1 + 2 * p;
                        if (ny < 0 || ny > 7)
                            continue;
                        int frees = 0;
                        if (positions[p, i, 0] > 0 && emptySquare(new int[] { positions[p, i, 0] - 1, ny }))
                            frees++;
                        if (emptySquare(new int[] { positions[p, i, 0], ny }))
                            frees++;
                        if (positions[p, i, 0] < 7 && emptySquare(new int[] { positions[p, i, 0] + 1, ny }))
                            frees++;
                        if (frees == 0)
                            res[p] -= 0.1f;
                        else
                            res[p] += frees * 0.01f;
                    }
            }
            float total = res[0] + res[1];
            if (total == 0)
                return base.Estimate();
            res[0] /= total;
            res[1] /= total;
            return res;
        }

        public override int GetNbPossibleMoves()
        {
            int n2 = 0;
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                    if (allowed[x, y])
                        n2++;
            return n2;
        }

        public override void DoPossibleMove(int n)
        {
            int n2 = 0;
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                    if (allowed[x, y])
                    {
                        if (n == n2)
                        {
                            moveTo(new int[] { x, y });
                            return;
                        }
                        n2++;
                    }
        }

        public override Game GetCopy(Game gameToCopyInto)
        {
            if (gameToCopyInto == null)
                gameToCopyInto = new MeepleBattleGame();
            gameToCopyInto = base.GetCopy(gameToCopyInto);
            MeepleBattleGame mbg = (MeepleBattleGame)gameToCopyInto;
            mbg.Score = (int[])Score.Clone();
            mbg.colors = (int[,])colors.Clone();
            mbg.positions = (int[, ,])positions.Clone();
            mbg.allowed = (bool[,])allowed.Clone();
            mbg.nextColor = nextColor;
            return mbg;
        }

        public override void ClickHere(double x, double y)
        {
            int[] position = getPositionFromRealPos((int)(1280 * x), (int)(876 * y));
            if (allowed[position[0], position[1]])
                moveTo(position);
            else
                showPossibleMoves = true;
        }

        private int[] getPositionFromRealPos(int x, int y)
        {
            x = x - (1280 / 2 - 876 / 2);
            x = x - (876 / 2 - 100 * 8 / 2);
            y = y - (876 / 2 - 100 * 8 / 2);
            return new int[] { x / 100, y / 100 };
        }

        private int getColorFromRealPos(int x, int y)
        {
            int[] position = getPositionFromRealPos(x, y);
            return colors[position[0], position[1]];
        }

        private bool allowedMove(int[] position)
        {
            if (!InGame || positions == null)
                return false;
            if (positions[NextPlayer, nextColor, 1] == position[1])
                return false;
            if (position[1] > positions[NextPlayer, nextColor, 1] && NextPlayer == 0)
                return false;
            if (position[1] < positions[NextPlayer, nextColor, 1] && NextPlayer == 1)
                return false;
            if (!emptySquare(position))
                return false;
            if (position[0] == positions[NextPlayer, nextColor, 0])
            {
                if (NextPlayer == 0)
                {
                    for (int y = positions[NextPlayer, nextColor, 1] - 1; y > position[1]; y--)
                        if (!emptySquare(new int[] { position[0], y }))
                            return false;
                    return true;
                }
                else
                {
                    for (int y = positions[NextPlayer, nextColor, 1] + 1; y < position[1]; y++)
                        if (!emptySquare(new int[] { position[0], y }))
                            return false;
                    return true;
                }
            }
            else if (position[0] < positions[NextPlayer, nextColor, 0])
            {
                if (NextPlayer == 0)
                {
                    int x = positions[NextPlayer, nextColor, 0] - 1;
                    for (int y = positions[NextPlayer, nextColor, 1] - 1;
                            y > position[1]; y--, x--)
                        if (!emptySquare(new int[] { x, y }))
                            return false;
                    return x == position[0];
                }
                else
                {
                    int x = positions[NextPlayer, nextColor, 0] - 1;
                    for (int y = positions[NextPlayer, nextColor, 1] + 1;
                            y < position[1]; y++, x--)
                        if (!emptySquare(new int[] { x, y }))
                            return false;
                    return x == position[0];
                }
            }
            else
            {
                if (NextPlayer == 0)
                {
                    int x = positions[NextPlayer, nextColor, 0] + 1;
                    for (int y = positions[NextPlayer, nextColor, 1] - 1;
                            y > position[1]; y--, x++)
                        if (!emptySquare(new int[] { x, y }))
                            return false;
                    return x == position[0];
                }
                else
                {
                    int x = positions[NextPlayer, nextColor, 0] + 1;
                    for (int y = positions[NextPlayer, nextColor, 1] + 1;
                            y < position[1]; y++, x++)
                        if (!emptySquare(new int[] { x, y }))
                            return false;
                    return x == position[0];
                }
            }
        }

        private void updateAllowedMoves()
        {
            int n = 0;
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                {
                    allowed[x, y] = allowedMove(new int[] { x, y });
                    if (allowed[x, y])
                        n++;
                }
            if (REGULAR_PASS && n == 0)
                allowed[positions[NextPlayer, nextColor, 0],
                    positions[NextPlayer, nextColor, 1]] = true;

        }

        private bool emptySquare(int[] position)
        {
            for (int p = 0; p < 2; p++)
                for (int i = 0; i < 8; i++)
                    if (position[0] == positions[p, i, 0] && position[1] == positions[p, i, 1])
                        return false;
            return true;
        }

        private double getFieldQuality()
        {
            int tot = 0;
            for (int x = 0; x < 8; x++)
                tot += getNbColorsFrom(x, false) + getNbColorsFrom(x, true);
            return Math.Sqrt(1.0 / (tot - 8 * 5));
        }

        private int getNbColorsFrom(int x, bool withVertical)
        {
            bool[] seen = new bool[8];
            for (int y = 1; y < 7; y++)
            {
                if(withVertical)
                    seen[colors[x, y]] = true;
                if (x - y >= 0)
                    seen[colors[x -y, y]] = true;
                if (x + y < 7)
                    seen[colors[x + y, y]] = true;
            }
            int res = 0;
            for (int i = 0; i < 8; i++)
                if (seen[i])
                    res++;
            return res;
        }

        private void moveTo(int[] position)
        {
            if (positions[NextPlayer, nextColor, 1] == position[1])
                nbNoMove++;
            else
                nbNoMove = 0;
            positions[NextPlayer, nextColor, 0] = position[0];
            positions[NextPlayer, nextColor, 1] = position[1];
            if ((position[1] == 0 && NextPlayer == 0) || (position[1] == 7 && NextPlayer == 1))
            {
                InGame = false;
                if (position[1] == 0)
                {
                    Score[0] = 1;
                    Score[1] = 0;
                }
                else
                {
                    Score[0] = 0;
                    Score[1] = 1;
                }
            }
            if (nbNoMove > 2) //draw
            {
                InGame = false;
                Score[0] = 1;
                Score[1] = 1;
            }
            nextColor = colors[position[0], position[1]];
            NextPlayer = 1 - NextPlayer;
            updateAllowedMoves();
            if (GetNbPossibleMoves() == 0)
            {
                InGame = false;
                Score[NextPlayer] = 0;
                Score[1 - NextPlayer] = 1;
            }
            showPossibleMoves = false;
        }
    }
}
