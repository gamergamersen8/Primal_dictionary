using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading.Tasks;
using System.IO;
using System.IO.Pipes;
using System.Diagnostics;
using System.Threading;
using System.Runtime.InteropServices;


class PipeServerFunc
{
    private NamedPipeServerStream pipeConnection;
    public async Task PipeSetup(string exePath = "Resources/Primal_dictionary_for_windows_cpp.exe")
    {
        // defining the executable for the primal dictionary
        try
        {
            ProcessStartInfo Si = new ProcessStartInfo
            {
                FileName = exePath,
                UseShellExecute = false,
                CreateNoWindow = true
            };

            using (Process process = Process.Start(Si))
            {
            }

            Console.WriteLine("Executable ran successfully");
        }
        catch (Exception ex)
        {
            Console.WriteLine("An error occurred: " + ex.Message);
        }
        pipeConnection = new NamedPipeServerStream("PrimalDictionary", PipeDirection.InOut, NamedPipeServerStream.MaxAllowedServerInstances, PipeTransmissionMode.Byte, PipeOptions.Asynchronous);
        Console.WriteLine("Waiting for client connection...");
        await pipeConnection.WaitForConnectionAsync();
        Console.WriteLine("Client connected.");
    }
    public async Task PipeSend(string message)
    {
        try
        {
            StreamWriter writer = new StreamWriter(pipeConnection);

            writer.WriteLine(message);
        }
        catch (System.InvalidOperationException) {
            MessageBox.Show("Error in connecting to base script.\ncheck the validity of the file Primal_dictionary_for_windows_cpp.exe");
        }
    }
    public async Task<string> PipeReceive()
    {
        try
        {
            StreamReader reader = new StreamReader(pipeConnection);

            string read = await reader.ReadToEndAsync();

            return read;
        }
        catch (System.InvalidOperationException)
        {
            return "Error in connecting to base script check the validity of the file Primal_dictionary_for_windows_cpp.exe";
        }
    }
    public async Task PipeClose()
    {
        pipeConnection.Close();
        pipeConnection = null;
    }
    ~PipeServerFunc()
    {
        pipeConnection.Close();
        pipeConnection = null;
    }
}
public class Form1 : Form
{
    private TextBox textBox1;
    private RichTextBox TranslationBox;
    public Button TranslateButton;
    public Font Primalfont = new Font("Primalfont", 10);
    public Font TimesNewRoman = new Font("Times New Roman", 10);
    PipeServerFunc pipeFunc = new PipeServerFunc();
    public Form1()
    {
        InitializeComponent();
    }
    /*
    public void WindowPattern(PaintEventArgs e)
    {
        //this silver blade has cut
        private string patternText = "Ru DFH fcn puT Yys";
    }*//*
    private async void Update()
    {

    }*/
    private async void translate(object sender, EventArgs e)
    {
        string TTBT = this.textBox1.Text;
        MessageBox.Show(TTBT);
        await pipeFunc.PipeSend(TTBT);

        string HBT = await pipeFunc.PipeReceive();
        MessageBox.Show(HBT);
        if (HBT.Contains("Error"))
        {
            MessageBox.Show(HBT);
        }
        else
        {
            MessageBox.Show(HBT);
            //this.TranslationBox.Text = HBT;
        }
    }
    private async Task InitializeComponent()
    {
        //
        // Setting up pipe
        //
        pipeFunc.PipeSetup();
        //
        // text around the window 
        //

        // 
        // textBox1
        // 

        this.textBox1 = new System.Windows.Forms.TextBox();
        this.SuspendLayout();

        this.textBox1.AcceptsReturn = true;
        this.textBox1.AcceptsTab = false;
        this.textBox1.Multiline = false;
        this.textBox1.Font = TimesNewRoman;
        this.textBox1.AutoSize = true;
        this.textBox1.TextAlign = HorizontalAlignment.Center;
        this.textBox1.Location = new Point((this.ClientSize.Width - this.textBox1.Width) / 2, 20);
        this.textBox1.BorderStyle = BorderStyle.FixedSingle;
        this.textBox1.BackColor = Color.FromArgb(50, 50, 50);
        this.textBox1.ForeColor = Color.White;
        this.textBox1.Text = "Hello";
        this.Controls.Add(this.textBox1);
        //
        // Translation button
        //
        this.TranslateButton = new System.Windows.Forms.Button();

        this.TranslateButton.Location = new Point((this.ClientSize.Width - this.TranslateButton.Width) / 2, 50);
        this.TranslateButton.Text = "Translate";
        this.TranslateButton.BackColor = Color.FromArgb(50, 50, 50);
        this.TranslateButton.ForeColor = Color.White;
        this.TranslateButton.Font = TimesNewRoman;

        //
        // Translation box
        //
        
        this.TranslationBox = new System.Windows.Forms.RichTextBox();
        this.TranslationBox.Dock = DockStyle.Fill;
        this.TranslationBox.ReadOnly = true;
        this.TranslationBox.BorderStyle = BorderStyle.FixedSingle;
        this.TranslationBox.BackColor = Color.FromArgb(50, 50, 50);
        this.TranslationBox.ForeColor = Color.White;
        this.TranslationBox.Location = new Point((this.ClientSize.Width - this.textBox1.Width) / 2, 80);

        // 
        // Form1
        // 

        this.ClientSize = new System.Drawing.Size(300, 250);
        this.FormBorderStyle = FormBorderStyle.FixedDialog;
        this.Controls.Add(this.TranslateButton);
        this.Controls.Add(this.textBox1);
        this.TranslateButton.Click += new EventHandler(translate);
        this.Controls.Add(this.TranslationBox);
        this.BackColor = Color.FromArgb(25, 25, 25);
        this.Text = "Primal dictionary";
        this.Icon = new Icon("/D:/c++/general/primal_dictionary/Logo.ico/"); //adding the logo for the top left of the window
        this.ResumeLayout(false);
        this.PerformLayout();
    }

    static void BudgetMain_Exit(object sender, EventArgs e)
    {
        DialogResult dialogResult = MessageBox.Show("Are You Sure You Want to Exit ?", "Warning", MessageBoxButtons.YesNo);
        if (dialogResult == DialogResult.Yes)
        {
            PipeServerFunc pipefunc = new PipeServerFunc();
            pipefunc.PipeClose();
            Application.Exit();
        }
    }

    [STAThread]
    static void Main()
    {
        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);

        Form1 form = new Form1();
        form.FormClosing += new FormClosingEventHandler(BudgetMain_Exit);

        form.Update();
        Application.Run(new Form1());
    }
}
