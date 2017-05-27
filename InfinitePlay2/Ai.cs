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
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace Askywhale.InfinitePlay
{
    delegate void AiDelegate();

    class Ai
    {
        public static string Name;

        protected Game game;
        protected int level;
        protected int num;
        protected Thread searchingThread;
        protected AiDelegate aiDelegate, aiBaseDelegate;
        protected bool searching, abortionWanted;

        static Ai()
        {
            Name = "?";
        }

        public Ai(Game game, int level, int num, AiDelegate aiBaseDelegate)
        {
            this.game = game;
            this.level = level;
            this.num = num;
            this.aiBaseDelegate = aiBaseDelegate;
            this.aiDelegate = aiDelegateNothing;
            searchingThread = null;
            abortionWanted = searching = false;
        }

        public void Execute()
        {
            lock (this)
            {
                bool shouldPlay = game.InGame && game.CanPlay[this.num];
                if (shouldPlay && !searching)
                {
                    //Console.WriteLine(num + " start");
                    searching = true;
                    aiDelegate = aiBaseDelegate;
                    abortionWanted = false;
                    searchingThread = new Thread(new ThreadStart(playThread));
                    searchingThread.Start();
                }
                if (!shouldPlay && searching)
                {
                  //  Console.WriteLine(num + " abort");
                    aiDelegate = aiDelegateNothing;
                    abortionWanted = true;                                        
                }
            }

        }

        //override me
        protected virtual int GetNextMove(){ return 0; }

        private void playThread()
        {
            int n = GetNextMove();
            lock (this)
            {
                //Console.WriteLine(num + " end");
                searching = false;
                if (n > -1 && !abortionWanted)
                {
                    game.DoPossibleMove(n);
                    new Thread(new ThreadStart(delegateThread)).Start();
                }
            }
        }

        private void delegateThread()
        {
            aiDelegate();            
        }

        private void aiDelegateNothing() { }
    }
}
