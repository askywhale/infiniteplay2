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
using System.Collections.Generic;

namespace Askywhale.InfinitePlay
{
    class Planet
    {
        public float x, y;
        public int[] nextP;
        public int type, player, num;
        public Planet(float x, float y, int type)
        {
            this.x = x;
            this.y = y;
            this.type = type;
            nextP = null;
            player = -1;
            num = 0;
        }
        public Planet Clone()
        {
            Planet p = new Planet(x, y, type);
            p.nextP = nextP; //do not need cloning
            p.player = player;
            p.num = num;
            return p;
        }        
    }

    class PlanetoidGame : Game
    {
        const int NB_FULLTURNS = 6;
        const int NB_START_SHIPS = 12;
        
        Planet[] planets;
        bool isSpreading;
        int stock, nbTurnsDone;
        int[] battleStock;

        public PlanetoidGame()
            : base()
        {
            Name = "Planetoid";
            Description = "Take planets one after one, in less than 6 turns.\n"+
                "Each turn, an attack phase, from planet to nearest ones, then "+
                "a spreading phase";
            WebURL = "http://askywhale.com/infiniteplay/";
            Version = 2;
            TreeDepth = 7;
            Approximation = 1.5f;

            InGame = false;
            planets = null;
        }

        public override int[] GetPossiblePlayers()
        {
            int[] res = { 2, 3, 4, 5, 6 };
            return res;
        }

        public override void StartGame(int nbPlayers, int totalTimeMs)
        {
            base.StartGame(nbPlayers, totalTimeMs);
            int nbPlanets = 10 + nbPlayers * 4;
            planets = new Planet[nbPlanets];
            for (int j = 0; j < nbPlanets; j++)
                planets[j] = new Planet(
                    (float)(Math.Sqrt(j) * Math.Cos(j) + random.NextDouble()),
                    (float)(Math.Sqrt(j) * Math.Sin(j) + random.NextDouble()),
                    random.Next(5));
            for(int i = 0; i<1e3; i++)
                for (int j = 0; j < nbPlanets; j++)
                    for (int k = 0; k < nbPlanets; k++)
                        if (j != k &&
                            Math.Sqrt((planets[j].x - planets[k].x) * (planets[j].x - planets[k].x) +
                                (planets[j].y - planets[k].y) * (planets[j].y - planets[k].y)) < i*.2)
                        {
                            planets[j].x += (planets[j].x - planets[k].x) * .5f;
                            planets[j].y += (planets[j].y - planets[k].y) * .5f;
                        }
            float minx = 1e6f, miny = 1e6f;
            float maxx = -1e6f, maxy = -1e6f;
            for (int j = 0; j < nbPlanets; j++)
            {
                if (minx > planets[j].x)
                    minx = planets[j].x;
                if (miny > planets[j].y)
                    miny = planets[j].y;
                if (maxx < planets[j].x)
                    maxx = planets[j].x;
                if (maxy < planets[j].y)
                    maxy = planets[j].y;
            }
            for (int j = 0; j < nbPlanets; j++)
            {
                planets[j].x = (planets[j].x - minx) / (maxx - minx);
                planets[j].y = (planets[j].y - miny) / (maxy - miny);
                planets[j].nextP = new int[10];
                for (int l = 0; l < 10; l++)
                    planets[j].nextP[l] = -1;
            }
            for (int j = 0; j < nbPlanets; j++)
                for (int k = 0; k < nbPlanets; k++)
                    if(j!=k && Math.Sqrt((planets[j].x - planets[k].x) * (planets[j].x - planets[k].x) +
                            (planets[j].y - planets[k].y) * (planets[j].y - planets[k].y)) < 
                            .56 - Math.Pow(nbPlanets,0.8) * .02f) 
                    {
                        for (int l = 0; l < 10; l++)
                            if (planets[j].nextP[l] == -1)
                            {
                                planets[j].nextP[l] = k;
                                break;
                            }                            
                    }
            for (int j = 0; j < nbPlanets; j++)
            {
                int l;
                for (l = 0; l < 10; l++)
                    if (planets[j].nextP[l] == -1)
                        break;
                int[] t = new int[l];
                for (int k = 0; k < l; k++)
                    t[k] = planets[j].nextP[k];
                planets[j].nextP = t;
            }
            battleStock = new int[nbPlayers];
            for (int i = 0; i < nbPlayers; i++)
            {
                for (int nj = 0; nj < nbPlanets; nj++)
                {
                    int j = nbPlanets - 1 - nj;
                    if (j > 3)
                        j -= i % 4;
                    if (planets[j].nextP.Length < 4 &&
                        planets[j].player == -1)
                    {
                        planets[j].player = i;
                        planets[j].num = NB_START_SHIPS;
                        Score[i] = 1;
                        if(planets[j].type == 2)
                            Score[i] ++;
                        break;
                    }
                }
                battleStock[i] = 0;
            }
            InGame = true;
            isSpreading = false;
            nbTurnsDone = 0;
            SetRandomNextPlayer();
            prepareStock();
        }

        public override void Paint()
        {
            drawImage("img.board.jpeg", 0, 0, 1, 1);
            if(NbPlayers > 0)
                for (int i = 0; i < NB_FULLTURNS; i++)
                {
                    String image = "img.turnnotdone.png";
                    if (i < nbTurnsDone / NbPlayers)
                        image = "img.turndone.png";
                    drawImage(image, 0, .04 * i, .05, .04);
                }
            if (planets != null)
            {
                foreach (Planet p in planets)
                    foreach (int n in p.nextP)
                    {
                        drawImage("img.dot.png",
                            (.1 + .8 * (p.x * .8 + planets[n].x * .2)),
                            (.1 + .8 * (p.y * .8 + planets[n].y * .2)),
                            .002);
                        drawImage("img.dot.png",
                            (.1 + .8 * (p.x * .68 + planets[n].x * .32)),
                            (.1 + .8 * (p.y * .68 + planets[n].y * .32)),
                            .002);
                        drawImage("img.dot.png",
                            (.1 + .8 * (p.x * .56 + planets[n].x * .44)),
                            (.1 + .8 * (p.y * .56 + planets[n].y * .44)),
                            .002);
                    }
                foreach (Planet p in planets)
                {
                    double size = .015 + p.type * .007;
                    double haloSize = size * 1.8;
                    if (p.player > -1)
                    {
                        if (!InGame && !Won[p.player])
                            haloSize *= .7;
                        if (p.player > -1)
                            drawImage("img.halo" + (p.player + 1) + ".png",
                                (.1 + .8 * p.x) - haloSize / 2,
                                (.1 + .8 * p.y) - haloSize * SCREEN_RATIO / 2, haloSize);
                    }
                    drawImage("img.planet" + (p.type + 1) + ".png",
                        (.1 + .8 * p.x) - size / 2,
                        (.1 + .8 * p.y) - size * SCREEN_RATIO / 2, size);
                    if (p.player > -1)
                        for (int i = 0; i < p.num; i++)
                            drawImage("img.ship" + (p.player + 1) + ".png",
                                (.1 + .8 * p.x) - .01 +
                                (p.type + 3) * 0.007 * Math.Cos(i),
                                (.1 + .8 * p.y) - .01 * SCREEN_RATIO +
                                (p.type + 3) * 0.007 * Math.Sin(i) * SCREEN_RATIO * .9, .02);
                }
            }
            if (InGame)
            {
                for (int i = 0; i < stock; i++)
                    drawImage("img.ship" + (NextPlayer + 1) + ".png",
                            .95, .9 - i * .03 - (i / 5) * .02, .03);
                if (isSpreading)
                    drawImage("img.spread.png", .95, .94, .03);
                else
                    drawImage("img.attack.png", .95, .95, .03);
            }
        }

        public override float[] Estimate()
        {
            int[] usedScore;
            if (InGame)
            {
                usedScore = (int[]) Score.Clone();
                int[][] nb = new int[NbPlayers][];
                for (int i = 0; i < NbPlayers; i++)
                    nb[i] = new int[5]{ 0, 0, 0, 0, 0 };
                foreach (Planet p in planets)
                    if (p.player > -1)
                        nb[p.player][p.type]++;
                for (int i = 0; i < NbPlayers; i++)
                {
                    usedScore[i] *= 20;                    
                    usedScore[i] += 
                        (NB_FULLTURNS - nbTurnsDone/NbPlayers) *
                        (int)(2 * Math.Min(nb[i][0], nb[i][1]) +
                            nb[i][3] +
                            2 * nb[i][4]);
                    int nTurn = NextPlayer - i;
                    if (nTurn < 0)
                        nTurn += NbPlayers;
                    if (nTurn == 0)
                    {
                        usedScore[i] += stock * 4;
                        if (!isSpreading)
                            usedScore[i] += 40;
                    }
                    else
                        usedScore[i] += (int)(80 * (1f - nTurn / (NbPlayers - 1)));
                }
                foreach (Planet p in planets)
                    if (p.player > -1)
                        usedScore[p.player] += (int)(5*Math.Sqrt(p.num));
            }
            else
            {
                usedScore = Score;
            }
            int total = 0;
            for (int i = 0; i < NbPlayers; i++)
                total += usedScore[i];
            if (total == 0)
                return base.Estimate();
            float[] res = new float[NbPlayers];
            for (int i = 0; i < NbPlayers; i++)
                res[i] = (float)usedScore[i] / total;
            return res;
        }

        public override int GetNbPossibleMoves()
        {
            int nb = 0;
            for (int i = 0; i < planets.Length; i++)
                if (mayClickOnPlanet(i))
                    nb++;
            return nb;
        }

        public override void DoPossibleMove(int n)
        {
            int nb = 0;
            for (int i = 0; i < planets.Length; i++)
                if (mayClickOnPlanet(i))
                    if (n == nb)
                    {
                        clickOnPlanet(i);
                        break;
                    }
                    else
                        nb++;
        }

        public override Game GetCopy(Game gameToCopyInto)
        {
            if (gameToCopyInto == null)
                gameToCopyInto = new PlanetoidGame();
            gameToCopyInto = base.GetCopy(gameToCopyInto);
            PlanetoidGame pg = (PlanetoidGame)gameToCopyInto;
            pg.planets = (Planet[])planets.Clone();
            for (int j = 0; j < planets.Length; j++)
                pg.planets[j] = pg.planets[j].Clone();
            pg.Score = (int[])Score.Clone();            
            pg.isSpreading = isSpreading;
            pg.stock = stock;
            pg.nbTurnsDone = nbTurnsDone;
            pg.battleStock = (int[])battleStock.Clone();
            return pg;
        }

        public override void ClickHere(double x, double y)
        {
            for(int j = 0; j < planets.Length ; j++)
            {                
                double xp = .1 + planets[j].x * .8;
                double yp = .1 + planets[j].y * .8;
                if (x > xp - .05 && x < xp + .05 &&
                        y > yp - .05 && y < yp + .05)
                    if (mayClickOnPlanet(j))
                        clickOnPlanet(j);
            }
        }

        private bool mayClickOnPlanet(int nPlanet)
        {
           Planet p = planets[nPlanet];
           if (p.player == NextPlayer)
               return true;
           if (isSpreading)
               return false;
           int needed = 2 + p.num;
           if (p.type == 3)
               needed++;
           if (p.type == 4)
               needed += 2;
           if (needed - 1 > stock)
               return false;
           bool hasBase = false, hasWay = false, hasEasyWay = false;
           foreach (Planet p2 in planets)
               if (p2.player == NextPlayer)
               {
                   hasBase = true;
                   foreach (int k in p2.nextP)
                       if (k == nPlanet)
                       {
                           hasWay = true;
                           if (p2.type == 4)
                               hasEasyWay = true;
                           break;
                       }
               }
           if (!hasWay && hasBase)
               return false;
           if (hasEasyWay)
               needed--;
           if (needed > stock)
               return false;
           return true;
        }

        private void clickOnPlanet(int nPlanet)
        {
            Planet p = planets[nPlanet];
            if (p.player == NextPlayer)
            {
                if (isSpreading)
                {
                    p.num++;
                    stock--;
                }
                else
                {
                    p.num += stock;
                    stock = 0;
                }
                mayChangePhaseOrTurn();
                return ;
            }
            int needed = 2+p.num;
            if(p.type == 3)
                needed++;
            if(p.type == 4)
                needed+=2;
            bool hasEasyWay = false;
            foreach (Planet p2 in planets)
                if (p2.player == NextPlayer)
                    foreach (int k in p2.nextP)
                        if (k == nPlanet)
                        {
                            if (p2.type == 4)
                                hasEasyWay = true;
                            break;
                        }
            if (hasEasyWay)
                needed--;
            if (p.player > -1)
            {
                battleStock[p.player] += p.num - 1;
                Score[p.player]--;
                if (p.type == 2)
                    Score[p.player]--;
            }
            p.player = NextPlayer;
            p.num = needed;
            stock -= needed;
            Score[p.player]++;
            if (p.type == 2)
                Score[p.player]++;
            mayChangePhaseOrTurn();
        }

        private void mayChangePhaseOrTurn()
        {
            if (stock == 0)
            {
                if (isSpreading)
                {                       
                    NextPlayer++;
                    nbTurnsDone++;
                    if (nbTurnsDone == NB_FULLTURNS * NbPlayers)
                        InGame = false;
                }
                isSpreading = !isSpreading;
                if (InGame)
                    prepareStock();
            }
        }

        private void prepareStock()
        {
            int np = NextPlayer;
            stock = 0;
            for(int j = 0; j<planets.Length ; j++)
                if (planets[j].player == np)
                {
                    stock += planets[j].num - 1;
                    planets[j].num = 1;
                }
            if (!isSpreading)
            {
                int[] nb = { 0, 0, 0, 0, 0 };
                foreach (Planet p in planets)
                    if (p.player == NextPlayer)
                        nb[p.type]++;
                stock += Math.Min(nb[0],nb[1]);
                stock += battleStock[np];
                battleStock[np] = 0;
                if (nb[0] + nb[1] + nb[2] + nb[3] + nb[4] == 0 && stock < 3)
                    stock = 3;
            }
        }
    }
}
