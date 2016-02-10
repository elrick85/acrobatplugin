using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using Acrobat;
using System.Windows.Forms;

namespace WinLb
{
	public interface ITestClass
	{
		string GetStr();
	}

	public class TestClass : ITestClass
	{
		private CAcroApp mApp;
		
		[DllImport("User32.dll")]
		public static extern int MessageBox(int h, string m, string c, int type);

		[DllImport("User32.dll")]
		public static extern IntPtr CreateWindow(
			string className,
			string windowName,
			uint style,
			int x,
			int y,
			int width,
			int height,
			IntPtr wndParent,
			IntPtr menu,
			IntPtr instance,
			IntPtr param);

		[DllImport("User32.dll")]
		public static extern int ShowWindow(IntPtr hwnd, int nCmdShow);

		[DllImport("User32.dll")]
		public static extern int UpdateWindow(IntPtr hwnd);
		
		public string GetStr()
		{
			//mApp = new AcroAppClass();

			//mApp.Lock();
			//CreateWindow("MainWClass", "Sample", Enums.WindowStyles.WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, null, null, null, null);

			var form = new MyForm();
			form.Show();

			//MessageBox(0, "API Message Box", "API Demo", 0);
			return "my string";
		}
	}
}
