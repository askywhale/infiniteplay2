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
using System.Text;

namespace Askywhale.InfinitePlay
{
    class SeniouAi:Ai
    {
        private static bool useAlphaBeta = true;

        static SeniouAi()
        {
            Name = "Seniou";
        }

        public SeniouAi(Game game, int level, int num, AiDelegate aiDelegate) : 
            base(game, level, num, aiDelegate) { }

        protected override int GetNextMove()
        {
            int l = 0;
            bool tooLate = false;
            int res = 0;
            long startStayingTime = game.StayingTimes[num];
            long lastUsedTime = 10;
            do
            {
                res = GetNextMoveLeveled(l);                
                lock (game.StayingTimes)
                {
                    tooLate = true;
                    for (int i = 0; i < game.NbPlayers; i++)
                        if (i != num && game.StayingTimes[i]*.8 <= game.StayingTimes[num])
                            tooLate = false;
                    long usedTime = startStayingTime - game.StayingTimes[num];
                    long nextUsedTime = usedTime * usedTime / Math.Max(1, Math.Min(usedTime, lastUsedTime));
                    if (usedTime + nextUsedTime > startStayingTime/1.5 ||
                            usedTime > game.TotalTimeMs / 10)
                        tooLate = true;
                    lock (aiDelegate)
                    {
                        if (abortionWanted)
                            tooLate = true;
                    }
                    lastUsedTime = usedTime;
                }
                l++;
            } while (l <= level && !tooLate);
            return res;
        }

        protected int GetNextMoveLeveled(int level)
        {
            int nbPossibleMoves = game.GetNbPossibleMoves();
            if (nbPossibleMoves == 1)
                return 0;
            float bestValue = -1e6f;
            int bestMove = -1;
            Game g = null;
            Random r = new Random();
            for (int n = 0; n < nbPossibleMoves ; n++)
            {
                g = game.GetCopy(g);
                g.DoPossibleMove(n);
                float[] estimates;
                if(useAlphaBeta)
                    estimates = getEstimatesRecA(g, game.NextPlayer, bestValue, level);
                else
                    estimates = getEstimatesRec(g, level);
                float value = estimates[game.NextPlayer];
                if (value > bestValue || 
                        (value == bestValue && r.NextDouble()>1/(n+1)) )
                {
                    bestValue = value;
                    bestMove = n;
                }
                lock (aiDelegate)
                {
                    if (abortionWanted)
                        break;
                }
            }
            return bestMove;
        }

        private float [] getEstimatesRec(Game game, int level)
        {
            if (level == 0 || !game.InGame)
            {
                return game.Estimate();
            }
            else
            {
                int nbPossibleMoves = game.GetNbPossibleMoves();
                if (nbPossibleMoves == 0)
                    return game.Estimate();
                float [] bestEstimates = null;
                Game g = null;
                for (int n = 0; n < nbPossibleMoves; n++)
                {
                    g = game.GetCopy(g);
                    g.DoPossibleMove(n);
                    float [] estimates = getEstimatesRec(g, level-1);
                    if (bestEstimates == null ||
                            estimates[game.NextPlayer] > bestEstimates[game.NextPlayer])
                        bestEstimates = estimates;
                }
                return bestEstimates;
            }            
        }

        private float[] getEstimatesRecA(Game game, int forWhoAlpha, float valueAlpha, int level)
        {
            if (level == 0 || !game.InGame)
            {
                return game.Estimate();
            }
            else
            {
                int nbPossibleMoves = game.GetNbPossibleMoves();
                if(nbPossibleMoves==0)
                    return game.Estimate();
                float bestValue = -1;
                float[] bestEstimates = null;
                Game g = null;
                for (int n = 0; n < nbPossibleMoves; n++)
                {
                    g = game.GetCopy(g);
                    g.DoPossibleMove(n);
                    float[] estimates = getEstimatesRecA(g, game.NextPlayer, bestValue, level - 1);
                    if (estimates[game.NextPlayer] > bestValue)
                    {
                        if (estimates[forWhoAlpha] < valueAlpha)
                            return estimates;
                        bestValue = estimates[game.NextPlayer];
                        bestEstimates = estimates;                        
                    }
                    lock (aiDelegate)
                    {
                        if (abortionWanted)
                            break;
                    }
                }                
                return bestEstimates;
            }
        }
    }
}
