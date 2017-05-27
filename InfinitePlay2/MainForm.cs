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
using System.Runtime.InteropServices;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Resources;
using System.Reflection;
using System.Threading;

namespace Askywhale.InfinitePlay
{
    public partial class MainForm : Form
    {
        [DllImport("user32.dll")]
        private static extern int FindWindow(string className, string windowText);
        [DllImport("user32.dll")]
        private static extern int ShowWindow(int hwnd, int command);
        [DllImport("user32.dll")]
        private static extern int SetWindowPos(IntPtr hwnd, IntPtr hwnd2, int x, int y, int cx, int cy, uint flags);
        [DllImport("user32.dll", EntryPoint = "GetSystemMetrics")]
        public static extern int GetSystemMetrics(int which);

        private const float sizeRatio = 1.461f;

        private List<Game> games;
        private Game game;
        private Ai[] ais;
        private int totalTimer;
        private int aiLevel;

        private System.Threading.Timer stayingTimeTimer;
        private DateTime lastStayingTimeTick;

        private Rectangle mainR, clockR;

        public MainForm()
        {
            InitializeComponent();
            findGames();
            loadSettings();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            calculateAreas();
        }

        private void StartGame()
        {
            ais = new Ai[game.NbPlayers];
            for (int i = 1; i < game.NbPlayers; i++)
            {
                if (aiLevel == 0)
                    ais[i] = new GadAi(game, 0, i, somethingHappenedOnBoard);
                else
                    ais[i] = new SeniouAi(game, aiLevel * game.TreeDepth / 4, i, somethingHappenedOnBoard);
            }
            game.StartGame(ais.Length, totalTimer * 1000);
            lastStayingTimeTick = DateTime.Now;
            stayingTimeTimer = new System.Threading.Timer(
                new TimerCallback(changeTime), null, 0, 200);
            somethingHappenedOnBoard();
        }

        private void stopGame()
        {
            if (game != null)
                game.InGame = false;
            if (ais != null)
                foreach (Ai ai in ais)
                    if (ai != null)
                        ai.Execute();
        }

        private void mainPanel_Paint(object sender, PaintEventArgs e)
        {
            game.CurrentForm = this;
            Bitmap mainB = new Bitmap(mainR.Width, mainR.Height);
            Bitmap clockB = new Bitmap(clockR.Width, clockR.Height);
            game.CurrentGraphics = Graphics.FromImage(mainB);
            game.Paint();
            drawClocks(Graphics.FromImage(clockB));
            e.Graphics.DrawImage(mainB, mainR);
            e.Graphics.DrawImage(clockB, clockR);
        }

        private void MainForm_Resize(object sender, EventArgs e)
        {
            FormBorderStyle newBorderStyle = FormBorderStyle.Sizable;
            if (WindowState == FormWindowState.Maximized)
                newBorderStyle = FormBorderStyle.None;
            if (this.FormBorderStyle != newBorderStyle)
            {
                this.FormBorderStyle = newBorderStyle;
                int hwnd = FindWindow("Shell_TrayWnd", "");
                if (WindowState == FormWindowState.Maximized)
                {
                    ShowWindow(hwnd, 0);
                    SetWindowPos(this.Handle, IntPtr.Zero, 0, 0,
                        Screen.PrimaryScreen.Bounds.Width,
                        Screen.PrimaryScreen.Bounds.Height, 64);
                }
                else
                {
                    ShowWindow(hwnd, 1);
                    Focus();
                }
            }
            fullScreenToolStripMenuItem.Checked = true;
            calculateAreas();
            somethingHappenedOnBoard();
        }

        private void calculateAreas()
        {
            float cw = MainPanel.ClientRectangle.Width;
            float ch = MainPanel.ClientRectangle.Height;
            if (cw / ch > sizeRatio * 1.1)
            { //right clock
                mainR = new Rectangle(0, 0, (int)(ch * sizeRatio), (int)ch);
                clockR = new Rectangle(mainR.Width, 0, (int)(cw - ch * sizeRatio), (int)ch);
            }
            else // top clock
            {
                clockR = new Rectangle(0, 0, (int)cw, (int)(ch - ch / sizeRatio));
                mainR = new Rectangle(0, clockR.Height, (int)cw, (int)(ch - clockR.Height));
            }
        }

        private void Form1_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Alt && e.KeyCode == Keys.Return)
                if (WindowState == FormWindowState.Maximized)
                    WindowState = FormWindowState.Normal;
                else
                    WindowState = FormWindowState.Maximized;
            if (ais != null && ais[game.NextPlayer] == null)
            {
                game.KeyPressed(e);
                somethingHappenedOnBoard();
            }
        }

        private void MainPanel_MouseUp(object sender, MouseEventArgs e)
        {
            if (ais != null && ais[game.NextPlayer] == null)
            {
                game.ClickHere((double)(e.X - mainR.X) / mainR.Width,
                    (double)(e.Y - mainR.Y) / mainR.Height);
                somethingHappenedOnBoard();
            }

        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (WindowState == FormWindowState.Maximized)
            {
                int hwnd = FindWindow("Shell_TrayWnd", "");
                ShowWindow(hwnd, 1);
            }
            stopGame();
            saveSettings();
        }

        private void somethingHappenedOnBoard()
        {
            MainPanel.Invalidate();
            Thread.Sleep(100); //justice
            if (ais != null)
                foreach (Ai ai in ais)
                    if (ai != null)
                        ai.Execute();
            if (!game.InGame && stayingTimeTimer != null)
            {
                stayingTimeTimer.Dispose();
                stayingTimeTimer = null;
            }
        }

        private void drawClocks(Graphics g)
        {
            float cw = g.VisibleClipBounds.Width;
            float ch = g.VisibleClipBounds.Height;
            bool hor = cw > ch;
            g.FillRectangle(Brushes.Black, 0, 0, cw, ch);

            for (int i = 0; i < game.NbPlayers; i++)
            {
                float px_time, py_time, fsize_time;
                Brush elapsedColor = new SolidBrush(Color.FromArgb(50, 50, 50));
                Brush stayingColor = new SolidBrush(Color.FromArgb(110, 110, 110));
                if (game.InGame && game.CanPlay != null && i < game.CanPlay.Length && game.CanPlay[i])
                {
                    elapsedColor = Brushes.Red;
                    stayingColor = Brushes.Green;
                }
                if (!game.InGame && i < game.Won.Length && game.Won[i] && ais[i] == null)
                {
                    elapsedColor = Brushes.Blue;
                    stayingColor = Brushes.LightBlue;
                }
                if (!game.InGame && i < game.Won.Length && game.Won[i] && ais[i] != null)
                {
                    elapsedColor = Brushes.Red;
                    stayingColor = Brushes.Pink;
                }
                if (!game.InGame && game.IsDraw)
                {
                    elapsedColor = Brushes.Orange;
                    stayingColor = Brushes.Yellow;
                }
                if (hor)
                {
                    g.FillRectangle(elapsedColor,
                        i * (int)(cw / game.NbPlayers) + 5, 5,
                        (int)(cw / game.NbPlayers - 10), ch - 10);
                    if (game.StayingTimes != null && i < game.StayingTimes.Length)
                        g.FillRectangle(stayingColor,
                            i * (int)(cw / game.NbPlayers) + 5, 5,
                            (int)((cw / game.NbPlayers - 10) *
                                game.StayingTimes[i] / game.TotalTimeMs), ch - 10);
                    fsize_time = (float)ch / 3;
                    px_time = (float)((i + .5) * (int)(cw / game.NbPlayers) - 1.5 * fsize_time);
                    py_time = (float)ch / 4;
                }
                else
                {
                    g.FillRectangle(elapsedColor,
                        5, (int)((game.NbPlayers - i - 1) * ch / game.NbPlayers) + 5,
                        cw - 10, (int)(ch / game.NbPlayers - 10));
                    if (game.StayingTimes != null && i < game.StayingTimes.Length)
                        g.FillRectangle(stayingColor,
                            5, (int)((game.NbPlayers - i - 1) * ch / game.NbPlayers) + 5,
                            cw - 10, (int)((ch / game.NbPlayers - 10) *
                                game.StayingTimes[i] / game.TotalTimeMs));
                    fsize_time = (float)cw / 5;
                    px_time = (float)cw / 4;
                    py_time = (float)(((game.NbPlayers - i - 1) + .5) * (int)(ch / game.NbPlayers) -
                        fsize_time / 2);
                }
                if (game.StayingTimes != null && i < game.StayingTimes.Length)
                    g.DrawString(
                        String.Format("{0:0}''{1:00}",
                            game.StayingTimes[i] / 60000,
                            (game.StayingTimes[i] / 1000) % 60),
                        new Font(FontFamily.GenericSansSerif, fsize_time),
                        new SolidBrush(Color.White),
                        px_time, py_time);
            }
        }

        private void changeTime(object o)
        {
            DateTime now = DateTime.Now;
            game.Tick((int)(now - lastStayingTimeTick).TotalMilliseconds);
            lastStayingTimeTick = DateTime.Now;
            somethingHappenedOnBoard();
        }

        private void findGames()
        {
            games = new List<Game>();
            chooseGameToolStripMenuItem1.DropDownItems.Clear();
            String path = Directory.GetCurrentDirectory() + "\\games\\";
            foreach (string file in Directory.GetFiles(path))
            {
                Assembly ass = Assembly.LoadFile(file);
                foreach (Type t in ass.GetTypes())
                {
                    if (t.IsSubclassOf(Type.GetType("Askywhale.InfinitePlay.Game")))
                    {
                        Game game2 = (Game)t.GetConstructor(Type.EmptyTypes).Invoke(null);
                        ToolStripMenuItem mi = new ToolStripMenuItem(game2.Name);
                        mi.Tag = t;
                        mi.Click += new System.EventHandler(chooseGame_Click);
                        games.Add(game2);
                        chooseGameToolStripMenuItem1.DropDownItems.Add(mi);
                    }
                }
            }
        }

        private void loadSettings()
        {
            // game name
            String gameType = (String)Properties.Settings.Default["OptionsGameType"];            
            ToolStripMenuItem firstItem = null;
            foreach (ToolStripMenuItem item in chooseGameToolStripMenuItem1.DropDownItems)
            {
                if (firstItem == null)
                    firstItem = item;
                if (((Type)item.Tag).FullName == gameType)
                    chooseGame_Click(item, null);
            }
            if (game == null)
                chooseGame_Click(firstItem, null);
            
            //nb players
            int nbPlayers = (int)Properties.Settings.Default["OptionsNbPlayers"];
            firstItem = null;
            foreach (ToolStripMenuItem item in playersToolStripMenuItem.DropDownItems)
            {
                if (firstItem == null)
                    firstItem = item;
                if (int.Parse(item.Text) == nbPlayers)
                    choosePlayers_Click(item, null);
            }            
            if (game.NbPlayers == 0)
                choosePlayers_Click(firstItem, null);
            
            //timer
            int optionTotalTimer = (int)Properties.Settings.Default["OptionsTotalTimer"];            
            firstItem = null;
            foreach (ToolStripMenuItem item in timerToolStripMenuItem.DropDownItems)
            {
                if (firstItem == null)
                    firstItem = item;
                if (int.Parse((string)(item.Tag)) == optionTotalTimer)
                    timerToolStripMenuItem_Click(item, null);
            }
            if (totalTimer == 0)
                timerToolStripMenuItem_Click(firstItem, null);

            //ai level
            int optionAiLevel = (int)Properties.Settings.Default["OptionsAiLevel"];
            firstItem = null;
            foreach (ToolStripMenuItem item in aiLevelToolStripMenuItem.DropDownItems)
            {
                if (firstItem == null)
                    firstItem = item;
                if (int.Parse((string)(item.Tag)) == optionAiLevel)
                    aiLevelToolStripMenuItem_Click(item, null);
            }
            if (aiLevel == 0)
                aiLevelToolStripMenuItem_Click(firstItem, null);
             
        }

        private void saveSettings()
        {
            Properties.Settings.Default["OptionsGameType"] = game.GetType().FullName;
            Properties.Settings.Default["OptionsNbPlayers"] = game.NbPlayers;
            Properties.Settings.Default["OptionsTotalTimer"] = totalTimer;
            Properties.Settings.Default["OptionsAiLevel"] = aiLevel;
            Properties.Settings.Default.Save();
        }

        private void fullScreenToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //MainForm_Resize(sender, e);
            if (WindowState == FormWindowState.Maximized)
                WindowState = FormWindowState.Normal;
            else
                WindowState = FormWindowState.Maximized;
        }

        private void quitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void chooseGame_Click(object sender, EventArgs e)
        {           
            Type t = (Type)((ToolStripMenuItem)sender).Tag;
            if (game == null ||  t != game.GetType())
                chooseGame(t);

            foreach (ToolStripMenuItem item in chooseGameToolStripMenuItem1.DropDownItems)
                item.Checked = false;
            ((ToolStripMenuItem)sender).Checked = true;
        }

        private void chooseGame(Type gameType)
        {
            stopGame();
            this.game = (Game)gameType.GetConstructor(Type.EmptyTypes).Invoke(null);
            game.InGame = false;
            ais = null;
            playersToolStripMenuItem.DropDownItems.Clear();
            foreach (int n in game.GetPossiblePlayers())
            {
                ToolStripMenuItem mi = new ToolStripMenuItem(n + "");
                mi.Click += new System.EventHandler(choosePlayers_Click);
                playersToolStripMenuItem.DropDownItems.Add(mi);
                if (game.NbPlayers == 0)
                    game.NbPlayers = n;
                if (game.NbPlayers == n)
                    mi.Checked = true;
            }

            somethingHappenedOnBoard();
        }

        private void choosePlayers_Click(object sender, EventArgs e)
        {
            int newNb = int.Parse(((ToolStripMenuItem)sender).Text);
            if (game != null && game.NbPlayers != newNb)
            {
                game = (Game)game.GetType().GetConstructor(Type.EmptyTypes).Invoke(null);
                game.NbPlayers = newNb;
            }

            foreach (ToolStripMenuItem item in playersToolStripMenuItem.DropDownItems)
                item.Checked = false;
            ((ToolStripMenuItem)sender).Checked = true;

            somethingHappenedOnBoard();
        }

        private void newGameToolStripMenuItem_Click(object sender, EventArgs e)
        {
            StartGame();
        }


        private void timerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            foreach (ToolStripMenuItem item in timerToolStripMenuItem.DropDownItems)
                item.Checked = false;
            ((ToolStripMenuItem)sender).Checked = true;
            totalTimer = int.Parse((string)(((ToolStripMenuItem)sender).Tag));
        }

        private void aiLevelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            foreach (ToolStripMenuItem item in aiLevelToolStripMenuItem.DropDownItems)
                item.Checked = false;
            ((ToolStripMenuItem)sender).Checked = true;
            aiLevel = int.Parse((string)(((ToolStripMenuItem)sender).Tag));
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("InfinitePlay 2.1.1\nhttps://askywhale.com/infiniteplay", 
                "About Infinite Play", 
                MessageBoxButtons.OK, 
                MessageBoxIcon.Information);
        }

    }
}
