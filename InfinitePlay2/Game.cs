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
using System.Drawing;
using System.Reflection;
using System.Windows.Forms;
using System.Threading;

namespace Askywhale.InfinitePlay
{
    public abstract class Game
    {
        protected const double SCREEN_RATIO = 1.4254;

        //name : name of your game
        public string Name;

        //description : a short description of your game
        public string Description;

        //weburl : an url (local or web) for a page relative to your game
        public string WebURL;

        //version : version of the plugin (in case of evolution of the interface)
        public int Version;

        //treeProof : depth you want ai at excellent level (ajust with playing, often 5..10)
        public int TreeDepth;

        //approximation : used by ai about the result of estimate()
        //default : 1, exponential (ie 0.5 opposed to 2, 0.2 to 5...)
        //increase if computer play often stupid move
        //decrease if all ai are all the same
        public float Approximation;

        //inGame : say if the game is in play (not finished)
        //if not, act if you want (actualize score[])
        // return true if game is in playing
        public bool InGame;

        //next player
        public int NbPlayers;

        //who can play now beyond players
        public bool[] CanPlay;

        //score : change this when the game finish (-oo...+oo)
        public int[] Score;

        //Time staying (in ms) for each player
        public long[] StayingTimes;
        
        //Total time of the game
        public long TotalTimeMs;

        public Graphics CurrentGraphics;
        public MainForm CurrentForm;

        protected Random random;

        protected Dictionary<String, Image> images;

        //called sometime before init()
        public Game()
        {
            Name = "?";
            Description = "?";
            WebURL = "http://askywhale.com/infiniteplay";
            Version = 1;
            TreeDepth = 5;
            Approximation = 1;

            InGame = false;
            CanPlay = null;
            Score = null;
            images = new Dictionary<String, Image>();
            CurrentGraphics = null;
            CurrentForm = null;
        }

        //getPossiblePlayers
        // return the size of the tab, and put in the tab the possible number of player
        public virtual int[] GetPossiblePlayers() { 
            int[] res = { 2 }; 
            return res; 
        }

        // newGame : begin a game
        public virtual void StartGame(int nbPlayers, int totalTimeMs) 
        {   
            NbPlayers = nbPlayers;
            TotalTimeMs = totalTimeMs;
            CanPlay = new bool[nbPlayers];
            Score = new int[nbPlayers];
            random = new Random();
            StayingTimes = new long[nbPlayers];
            for (int i = 0; i < nbPlayers; i++)
            {
                StayingTimes[i] = totalTimeMs;
                CanPlay[i] = false;
                Score[i] = 0;
            }
            CanPlay[0] = true;
        }

        //do the paint
        public virtual void Paint() { }
        
        // surrender : a played surrender
        // return false is surrendering is not autorized
        public virtual bool Surrender(int player) {
            InGame = false;
            Score[player] = 0;
            return true;
        }

        //estimate : used by ai
        // r[n] = 0..1, 0 if catastrophic situation for player n (regarding other r[i])
        //              1 if perfect situation for player n (regarding other r[i])
        // total(r) : 1
        public virtual float[] Estimate() { return new float[]{ .5f, .5f }; }

        //return number of possible move from now ; used by ai
        public virtual int GetNbPossibleMoves() { return 1; }

        //newMove : used by ai : do move number n (change the situation)        
        public virtual void DoPossibleMove(int n) { }

        //return -1 if ai should be used
        public virtual int GetBestNextMove() { return -1; }        

        //GetCopy from..
        public virtual Game GetCopy(Game gameToCopyInto)
        {
            if (gameToCopyInto == null)
                return null;
            gameToCopyInto.InGame = InGame;
            gameToCopyInto.NbPlayers = NbPlayers;
            gameToCopyInto.CanPlay = (bool[])CanPlay.Clone();
            gameToCopyInto.Score = (int[])Score.Clone();
            gameToCopyInto.images = images;
            gameToCopyInto.CurrentGraphics = CurrentGraphics;
            gameToCopyInto.CurrentForm = CurrentForm;
            return gameToCopyInto;
        }

        //click here : use when clicked
        //x,y : position (max:1,1)
        public virtual void ClickHere(double x, double y) { }

        //touchPressed : similar to clickHere
        //n : number of the key
        public virtual void KeyPressed(KeyEventArgs e) { }

        public int NextPlayer
        {
            get
            {
                for (int i = 0; i < NbPlayers; i++)
                    if (CanPlay[i])
                        return i;
                return -1;
            }
            set
            {
                value %= NbPlayers;
                for (int i = 0; i < NbPlayers; i++)
                    CanPlay[i] = false;
                CanPlay[value] = true;
            }
        }

        public int SetRandomNextPlayer()
        {
            int nextPlayer = random.Next(NbPlayers);
            NextPlayer = nextPlayer;
            return nextPlayer;
        }

        public bool IsDraw
        {
            get
            {
                int best = -100;
                if (Score != null)
                    foreach (int s in Score)
                        if (s > best)
                            best = s;
                int nbAtBest = 0;
                if (Score != null)
                    for (int n = 0; n < NbPlayers; n++)
                        if (Score[n] == best)
                            nbAtBest++;
                return nbAtBest == NbPlayers;
            }
        }

        public bool[] Won
        {
            get
            {
                int best = -100;
                if (Score != null)
                    foreach(int s in Score)
                        if(s > best)
                            best = s;
                bool[] res = new bool[NbPlayers];
                if (Score != null)
                    for (int n = 0; n < NbPlayers; n++)
                        res[n] = Score[n] == best;
                return res;
            }
        }

        //use it to draw
        protected void drawImage(Bitmap bitmap, double x, double y, double w)
        {
            double h = w * bitmap.Height / bitmap.Width;
            drawImage(bitmap, x, y, w, h);
        }

        protected void drawImage(Bitmap bitmap, double x, double y, double w, double h)
        {
            float cw = CurrentGraphics.VisibleClipBounds.Width;
            float ch = CurrentGraphics.VisibleClipBounds.Height;
            CurrentGraphics.DrawImage(bitmap,
                (float)(cw * x), (float)(ch * y),
                (float)(cw * w), (float)(ch * h));
        }

        protected void drawImage(string name, double x, double y, double w)
        {
            createImageIfNeeded(name);
            float cw = CurrentGraphics.VisibleClipBounds.Width;
            float ch = CurrentGraphics.VisibleClipBounds.Height;
            double h = w * images[name].Height / images[name].Width;
            CurrentGraphics.DrawImage(images[name],
                (float)(cw * x), (float)(ch * y),
                (float)(cw * w), (float)(cw * h));
        }

        protected void drawImage(string name, double x, double y, double w, double h)
        {
            createImageIfNeeded(name);
            float cw = CurrentGraphics.VisibleClipBounds.Width;
            float ch = CurrentGraphics.VisibleClipBounds.Height;
            CurrentGraphics.DrawImage(images[name],
                (float)(cw * x), (float)(ch * y),
                (float)(cw * w), (float)(ch * h));
        }

        //use it to draw
        protected void drawString(string text, Font font, Brush brush, double x, double y)
        {
            float cw = CurrentGraphics.VisibleClipBounds.Width;
            float ch = CurrentGraphics.VisibleClipBounds.Height;
            if (CurrentGraphics != null && CurrentForm != null)
            {
                font = new Font(font.FontFamily,
                    font.Size * CurrentForm.ClientRectangle.Width / 400,
                    font.Style);
                CurrentGraphics.DrawString(text, font, brush,
                    (float)(cw * x), (float)(ch * y));
            }
        }

        public void Tick(long elapsed)
        {
            lock (StayingTimes)
            {
                DateTime now = DateTime.Now;
                for (int i = 0; i < NbPlayers && InGame; i++)
                    if (CanPlay[i])
                        if (StayingTimes[i] < elapsed)
                        {
                            StayingTimes[i] = 0;
                            for (int j = 0; j < NbPlayers; j++)
                                if (Score[j] == 0)
                                    Score[j] = 1;
                            Score[i] = 0;
                            InGame = false;
                        }
                        else
                        {
                            StayingTimes[i] -= elapsed;
                        }
            }
        }

        protected void createImageIfNeeded(string name)
        {
            if (!images.ContainsKey(name))
            {
                Assembly currentAssembly = System.Reflection.Assembly.GetAssembly(this.GetType());
                /*                string caName = currentAssembly.FullName.Substring(0,
                                    currentAssembly.FullName.IndexOf(",")); */
                string[] rnames = currentAssembly.GetManifestResourceNames();
                foreach (string rname in rnames)
                    if (rname.EndsWith(name))
                    {
                        images[name] = Image.FromStream(currentAssembly.
                            GetManifestResourceStream(rname));
                        break;
                    }
            }
        }
    }

}
