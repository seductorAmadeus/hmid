using System;
using System.Windows.Forms;
using System.Diagnostics;

namespace lab_5
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private MyProcess _myProcess;
        private int _counter;
        private void button1_Click(object sender, EventArgs e)
        {
            button1.Enabled = false;
            _myProcess = new MyProcess();
            _myProcess.BindToRunningProcesses();

            _counter = 0;
            foreach (var i in _myProcess.ProcessList)
            {
                if (_counter > 10)
                    break;
                tableLayoutPanel1.RowCount = tableLayoutPanel1.RowCount + 1;
                var l1 = new Label
                {
                    Text = i.ProcessName,
                    AutoSize = true
                };
                tableLayoutPanel1.Controls.Add(l1, 0, tableLayoutPanel1.RowCount - 1);
                Console.WriteLine(i.ProcessName);

                l1 = new Label
                {
                    Text = i.Id.ToString(),
                    AutoSize = true
                };
                tableLayoutPanel1.Controls.Add(l1, 1, tableLayoutPanel1.RowCount - 1);
                Console.WriteLine(i.Id.ToString());

                l1 = new Label
                {
                    Text = i.PrivateMemorySize64.ToString(),
                    AutoSize = true
                };
                tableLayoutPanel1.Controls.Add(l1, 2, tableLayoutPanel1.RowCount - 1);

                l1 = new Label
                {
                    Text = _myProcess.GetNumberOfMemoryPages(i).ToString(),
                    AutoSize = true
                };
                tableLayoutPanel1.Controls.Add(l1, 3, tableLayoutPanel1.RowCount - 1);
                _counter++;
            }
        }
    }
}
