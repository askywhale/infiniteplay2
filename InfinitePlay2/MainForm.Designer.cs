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
namespace Askywhale.InfinitePlay
{
    partial class MainForm
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Code généré par le Concepteur Windows Form


        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newGameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
            this.quitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.gameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.chooseGameToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.playersToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.optionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fullScreenToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.timerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.hourToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.minutes20ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.minutes10ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.minutes5ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.minutes2ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.minute1ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aiLevelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.randomToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.veryWeakToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.weakToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.strongToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.veryStrongToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.MainPanel = new Askywhale.InfinitePlay.DblBufferedPanel();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.gameToolStripMenuItem,
            this.optionsToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(892, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newGameToolStripMenuItem,
            this.toolStripMenuItem3,
            this.quitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // newGameToolStripMenuItem
            // 
            this.newGameToolStripMenuItem.Name = "newGameToolStripMenuItem";
            this.newGameToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
            this.newGameToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.newGameToolStripMenuItem.Text = "&New game";
            this.newGameToolStripMenuItem.Click += new System.EventHandler(this.newGameToolStripMenuItem_Click);
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(160, 6);
            // 
            // quitToolStripMenuItem
            // 
            this.quitToolStripMenuItem.Name = "quitToolStripMenuItem";
            this.quitToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.F4)));
            this.quitToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.quitToolStripMenuItem.Text = "&Quit";
            this.quitToolStripMenuItem.Click += new System.EventHandler(this.quitToolStripMenuItem_Click);
            // 
            // gameToolStripMenuItem
            // 
            this.gameToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.chooseGameToolStripMenuItem1,
            this.toolStripMenuItem2,
            this.playersToolStripMenuItem});
            this.gameToolStripMenuItem.Name = "gameToolStripMenuItem";
            this.gameToolStripMenuItem.Size = new System.Drawing.Size(46, 20);
            this.gameToolStripMenuItem.Text = "&Game";
            // 
            // chooseGameToolStripMenuItem1
            // 
            this.chooseGameToolStripMenuItem1.Name = "chooseGameToolStripMenuItem1";
            this.chooseGameToolStripMenuItem1.Size = new System.Drawing.Size(139, 22);
            this.chooseGameToolStripMenuItem1.Text = "&Choose game";
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(136, 6);
            // 
            // playersToolStripMenuItem
            // 
            this.playersToolStripMenuItem.Name = "playersToolStripMenuItem";
            this.playersToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.playersToolStripMenuItem.Text = "Players";
            // 
            // optionsToolStripMenuItem
            // 
            this.optionsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fullScreenToolStripMenuItem,
            this.timerToolStripMenuItem,
            this.aiLevelToolStripMenuItem});
            this.optionsToolStripMenuItem.Name = "optionsToolStripMenuItem";
            this.optionsToolStripMenuItem.Size = new System.Drawing.Size(56, 20);
            this.optionsToolStripMenuItem.Text = "&Options";
            // 
            // fullScreenToolStripMenuItem
            // 
            this.fullScreenToolStripMenuItem.Name = "fullScreenToolStripMenuItem";
            this.fullScreenToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F10;
            this.fullScreenToolStripMenuItem.Size = new System.Drawing.Size(151, 22);
            this.fullScreenToolStripMenuItem.Text = "Full Screen";
            this.fullScreenToolStripMenuItem.Click += new System.EventHandler(this.fullScreenToolStripMenuItem_Click);
            // 
            // timerToolStripMenuItem
            // 
            this.timerToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.hourToolStripMenuItem,
            this.minutes20ToolStripMenuItem,
            this.minutes10ToolStripMenuItem,
            this.minutes5ToolStripMenuItem,
            this.minutes2ToolStripMenuItem,
            this.minute1ToolStripMenuItem});
            this.timerToolStripMenuItem.Name = "timerToolStripMenuItem";
            this.timerToolStripMenuItem.Size = new System.Drawing.Size(151, 22);
            this.timerToolStripMenuItem.Text = "&Timer";
            // 
            // hourToolStripMenuItem
            // 
            this.hourToolStripMenuItem.Name = "hourToolStripMenuItem";
            this.hourToolStripMenuItem.Size = new System.Drawing.Size(126, 22);
            this.hourToolStripMenuItem.Tag = "3600";
            this.hourToolStripMenuItem.Text = "1 hour";
            this.hourToolStripMenuItem.Click += new System.EventHandler(this.timerToolStripMenuItem_Click);
            // 
            // minutes20ToolStripMenuItem
            // 
            this.minutes20ToolStripMenuItem.Name = "minutes20ToolStripMenuItem";
            this.minutes20ToolStripMenuItem.Size = new System.Drawing.Size(126, 22);
            this.minutes20ToolStripMenuItem.Tag = "1200";
            this.minutes20ToolStripMenuItem.Text = "20 minutes";
            this.minutes20ToolStripMenuItem.Click += new System.EventHandler(this.timerToolStripMenuItem_Click);
            // 
            // minutes10ToolStripMenuItem
            // 
            this.minutes10ToolStripMenuItem.Name = "minutes10ToolStripMenuItem";
            this.minutes10ToolStripMenuItem.Size = new System.Drawing.Size(126, 22);
            this.minutes10ToolStripMenuItem.Tag = "600";
            this.minutes10ToolStripMenuItem.Text = "10 minutes";
            this.minutes10ToolStripMenuItem.Click += new System.EventHandler(this.timerToolStripMenuItem_Click);
            // 
            // minutes5ToolStripMenuItem
            // 
            this.minutes5ToolStripMenuItem.Name = "minutes5ToolStripMenuItem";
            this.minutes5ToolStripMenuItem.Size = new System.Drawing.Size(126, 22);
            this.minutes5ToolStripMenuItem.Tag = "300";
            this.minutes5ToolStripMenuItem.Text = "5 minutes";
            this.minutes5ToolStripMenuItem.Click += new System.EventHandler(this.timerToolStripMenuItem_Click);
            // 
            // minutes2ToolStripMenuItem
            // 
            this.minutes2ToolStripMenuItem.Name = "minutes2ToolStripMenuItem";
            this.minutes2ToolStripMenuItem.Size = new System.Drawing.Size(126, 22);
            this.minutes2ToolStripMenuItem.Tag = "120";
            this.minutes2ToolStripMenuItem.Text = "2 minutes";
            this.minutes2ToolStripMenuItem.Click += new System.EventHandler(this.timerToolStripMenuItem_Click);
            // 
            // minute1ToolStripMenuItem
            // 
            this.minute1ToolStripMenuItem.Name = "minute1ToolStripMenuItem";
            this.minute1ToolStripMenuItem.Size = new System.Drawing.Size(126, 22);
            this.minute1ToolStripMenuItem.Tag = "60";
            this.minute1ToolStripMenuItem.Text = "1 minute";
            this.minute1ToolStripMenuItem.Click += new System.EventHandler(this.timerToolStripMenuItem_Click);
            // 
            // aiLevelToolStripMenuItem
            // 
            this.aiLevelToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.randomToolStripMenuItem,
            this.veryWeakToolStripMenuItem,
            this.weakToolStripMenuItem,
            this.strongToolStripMenuItem,
            this.veryStrongToolStripMenuItem});
            this.aiLevelToolStripMenuItem.Name = "aiLevelToolStripMenuItem";
            this.aiLevelToolStripMenuItem.Size = new System.Drawing.Size(151, 22);
            this.aiLevelToolStripMenuItem.Text = "&AI Level";
            // 
            // randomToolStripMenuItem
            // 
            this.randomToolStripMenuItem.Name = "randomToolStripMenuItem";
            this.randomToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
            this.randomToolStripMenuItem.Tag = "0";
            this.randomToolStripMenuItem.Text = "Random";
            this.randomToolStripMenuItem.Click += new System.EventHandler(this.aiLevelToolStripMenuItem_Click);
            // 
            // veryWeakToolStripMenuItem
            // 
            this.veryWeakToolStripMenuItem.Name = "veryWeakToolStripMenuItem";
            this.veryWeakToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
            this.veryWeakToolStripMenuItem.Tag = "1";
            this.veryWeakToolStripMenuItem.Text = "Very weak";
            this.veryWeakToolStripMenuItem.Click += new System.EventHandler(this.aiLevelToolStripMenuItem_Click);
            // 
            // weakToolStripMenuItem
            // 
            this.weakToolStripMenuItem.Name = "weakToolStripMenuItem";
            this.weakToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
            this.weakToolStripMenuItem.Tag = "2";
            this.weakToolStripMenuItem.Text = "Weak";
            this.weakToolStripMenuItem.Click += new System.EventHandler(this.aiLevelToolStripMenuItem_Click);
            // 
            // strongToolStripMenuItem
            // 
            this.strongToolStripMenuItem.Name = "strongToolStripMenuItem";
            this.strongToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
            this.strongToolStripMenuItem.Tag = "3";
            this.strongToolStripMenuItem.Text = "Strong";
            this.strongToolStripMenuItem.Click += new System.EventHandler(this.aiLevelToolStripMenuItem_Click);
            // 
            // veryStrongToolStripMenuItem
            // 
            this.veryStrongToolStripMenuItem.Name = "veryStrongToolStripMenuItem";
            this.veryStrongToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
            this.veryStrongToolStripMenuItem.Tag = "4";
            this.veryStrongToolStripMenuItem.Text = "Very strong";
            this.veryStrongToolStripMenuItem.Click += new System.EventHandler(this.aiLevelToolStripMenuItem_Click);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aboutToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(40, 20);
            this.helpToolStripMenuItem.Text = "&Help";
            // 
            // aboutToolStripMenuItem
            // 
            this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            this.aboutToolStripMenuItem.Size = new System.Drawing.Size(115, 22);
            this.aboutToolStripMenuItem.Text = "&About...";
            this.aboutToolStripMenuItem.Click += new System.EventHandler(this.aboutToolStripMenuItem_Click);
            // 
            // MainPanel
            // 
            this.MainPanel.BackColor = System.Drawing.SystemColors.GradientInactiveCaption;
            this.MainPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MainPanel.Location = new System.Drawing.Point(0, 24);
            this.MainPanel.Name = "MainPanel";
            this.MainPanel.Size = new System.Drawing.Size(892, 549);
            this.MainPanel.TabIndex = 1;
            this.MainPanel.Paint += new System.Windows.Forms.PaintEventHandler(this.mainPanel_Paint);
            this.MainPanel.MouseUp += new System.Windows.Forms.MouseEventHandler(this.MainPanel_MouseUp);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(892, 573);
            this.Controls.Add(this.MainPanel);
            this.Controls.Add(this.menuStrip1);
            this.DoubleBuffered = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "MainForm";
            this.Text = "Infinite play 2";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Form1_KeyUp);
            this.Resize += new System.EventHandler(this.MainForm_Resize);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newGameToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem quitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem gameToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem chooseGameToolStripMenuItem1;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem playersToolStripMenuItem;
        private DblBufferedPanel MainPanel;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem3;
        private System.Windows.Forms.ToolStripMenuItem optionsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem timerToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem hourToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem minutes20ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem minutes10ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem minutes5ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem minutes2ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem minute1ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aiLevelToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem randomToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem veryWeakToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem weakToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem strongToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem veryStrongToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem fullScreenToolStripMenuItem;



    }
}

