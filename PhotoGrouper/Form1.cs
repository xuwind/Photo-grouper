using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PhotoGrouper
{
	public partial class Form1 : Form
	{
        private int filesCopied=0, filesDone = 0, filesTotal = 0;
        private string fromFolder, toFolder;

        public Form1()
		{
			InitializeComponent();
		}

		private async void button1_Click(object sender, EventArgs e)
		{
            if (string.IsNullOrWhiteSpace(textBox2.Text) || string.IsNullOrWhiteSpace(textBox3.Text))
            {
                MessageBox.Show("please select a valid source and destination folders to process!");
                return;
            }
            fromFolder = textBox3.Text.Trim();
            toFolder = textBox2.Text.Trim();
            if (!Directory.Exists(fromFolder)) Directory.CreateDirectory(fromFolder);
            if (!Directory.Exists(toFolder)) Directory.CreateDirectory(toFolder);
            progressBar1.Value = 0;
            var progress = new Progress<int>(percent =>
			{
				progressBar1.Value = percent;
                linkLabel1.Text = filesDone + " / " + filesTotal;
                linkLabel1.Invalidate();
			});

			string result = await Grouping(progress);

			MessageBox.Show(result);
		}

		private List<string> getAllImages(string path)
		{
			var files = Directory.GetFiles(path, "*.*", SearchOption.AllDirectories);

			List<string> imageFiles = new List<string>();
			foreach (string filename in files)
			{
				if (Regex.IsMatch(filename, @"(?i).jpg|.png|.gif|.bmp|.heic"))
					imageFiles.Add(filename);
			};
			return imageFiles;
		}

		private void textBox1_DoubleClick(object sender, EventArgs e)
		{
			openFileDialog1.ShowDialog();
		}

		private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
		{
			textBox1.Text = openFileDialog1.FileName;
			if (textBox1.Text.Trim().Length > 0)
			{
				var grouper = new ImageInfo(textBox1.Text);
				linkLabel1.Text = grouper.LocationUrl;
				listBox1.Items.Clear();
				listBox1.Items.Add("Taken by : "+ grouper.TakenBy);
				listBox1.Items.Add("Taken Date : " + grouper.TakenDate?.ToString("yyyy-MM-dd"));
                listBox1.Items.Add("Destination : " + grouper.Destination);
            }
		}

		private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
		{
			ProcessStartInfo sInfo = new ProcessStartInfo(linkLabel1.Text);
			Process.Start(sInfo);
		}

		private void textBox3_DoubleClick(object sender, EventArgs e)
		{
			textBox3.Text = selectFolder();
		}

        private string selectFolder()
		{
			using (FolderBrowserDialog dlg = new FolderBrowserDialog())
			{
				if (dlg.ShowDialog() != DialogResult.OK)
				{
					return "";
				}
				return dlg.SelectedPath;
			}
		}
	
		private void textBox2_DoubleClick(object sender, EventArgs e)
		{
			textBox2.Text = selectFolder();
		}

  		async Task<string> Grouping(IProgress<int> progress)
		{
			filesDone = 0;
			filesTotal = 0;
			List<string> fileList = getAllImages(textBox3.Text);
			filesTotal = fileList.Count();
			foreach(var file in fileList)
			{
                bool cright = await CopyFile(file);
                if (cright) filesCopied++;
				filesDone++;
                if (filesDone % 5 == 0 || filesDone == filesTotal)
                {
                    var pro = (int)((double)filesDone * 100 / filesTotal);
                    progress.Report(pro);
                }
			}
            return "grouping completed";
		}

        async Task<bool> CopyFile(string filePath)
        {
            var ifo = new  ImageInfo(filePath);
            string to = toFolder + "\\" + ifo.Destination;
            if (!Directory.Exists(to)) Directory.CreateDirectory(to);
            to += "\\" + ifo.FileName;
            if (!File.Exists(to))
            {
                var sourceStream = new FileStream(filePath, FileMode.Open, FileAccess.Read, FileShare.Read, 4096, FileOptions.Asynchronous | FileOptions.SequentialScan);
                var destinationStream = new FileStream(to, FileMode.CreateNew, FileAccess.Write, FileShare.None, 4096, FileOptions.Asynchronous | FileOptions.SequentialScan);
                await sourceStream.CopyToAsync(destinationStream);
                sourceStream.Dispose();
                destinationStream.Dispose();
            }
            ifo = null;
            return true;
        } 
    }
}
