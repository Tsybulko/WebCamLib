using System;
using System.Runtime.InteropServices;
using System.Text;
namespace WebLib_1
{
    public class Constants {
        public const uint WM_CAP = 0x400;
        public const uint WM_CAP_DRIVER_CONNECT = 0x40a;
        public const uint WM_CAP_DRIVER_DISCONNECT = 0x40b;
        public const uint WM_CAP_EDIT_COPY = 0x41e;
        public const uint WM_CAP_SET_PREVIEW = 0x432;
        public const uint WM_CAP_SET_OVERLAY = 0x433;
        public const uint WM_CAP_SET_PREVIEWRATE = 0x434;
        public const uint WM_CAP_SET_SCALE = 0x435;
        public const uint WS_CHILD = 0x40000000;
        public const uint WS_VISIBLE = 0x10000000;
    }
	
	public class Avicap32
    {
        [DllImport("avicap32.dll")]
        public extern static IntPtr capGetDriverDescription(
            ushort index,
            StringBuilder name,
            int nameCapacity,
            StringBuilder description,
            int descriptionCapacity
        );

        [DllImport("avicap32.dll")]
        public extern static IntPtr capCreateCaptureWindow(
            string title,
            uint style,
            int x,
            int y,
            int width,
            int height,
            IntPtr window,
            int id
        );
    }
	
	
	public class User32
    {
        [DllImport("user32.dll")]
        public static extern IntPtr SendMessage(
            IntPtr hWnd, 
            uint Msg, 
            IntPtr wParam, 
            IntPtr lParam
        );

        [DllImport("user32.dll")]
        public static extern IntPtr SetWindowPos(
            IntPtr hWnd, 
            IntPtr hWndInsertAfter, 
            int X,
            int Y, 
            int cx, 
            int cy, 
            uint uFlags
        );

        [DllImport("user32")]
        public static extern IntPtr DestroyWindow(
            IntPtr hWnd
        );
    }
	public class CaptureDevice
	{
	private static int MAX_DEVICES = 10;
	private ushort deviceNumber;
	private string name;
	private string description;
	private IntPtr deviceHandle;

	

	public CaptureDevice (ushort deviceNumber, string name, string description) 
	{
		this.deviceNumber = deviceNumber;
		this.name = name;
		this.description = description;
	}

	public ushort DeviceNumber {
		get { return deviceNumber; }
		set { deviceNumber = value; }
	}

	public string Name {
		get { return name; }
		set { name = value; }
	}

	public string Description {
		get { return description; }
		set { description = value; }
	}

	
		
		public void Attach(int Width, int Height ,System.IntPtr Handle)
        {
           deviceHandle = Avicap32.capCreateCaptureWindow("", Constants.WS_VISIBLE | Constants.WS_CHILD, 0, 0,Width, Height, Handle, 0);
	   	
			
            if (User32.SendMessage(deviceHandle, Constants.WM_CAP_DRIVER_CONNECT, (IntPtr)deviceNumber, (IntPtr)0).ToInt32() > 0)
            {
                User32.SendMessage(deviceHandle, Constants.WM_CAP_SET_SCALE, (IntPtr)(-1), (IntPtr)0);
                User32.SendMessage(deviceHandle, Constants.WM_CAP_SET_PREVIEWRATE, (IntPtr)0x42, (IntPtr)0);
                User32.SendMessage(deviceHandle, Constants.WM_CAP_SET_PREVIEW, (IntPtr)(-1), (IntPtr)0);
                User32.SetWindowPos(deviceHandle, new IntPtr(0), 0, 0, Width, Height, 6);
            }
        }


	
	   public void Detach()
        {
            if (deviceHandle.ToInt32() != 0)
            {
                User32.SendMessage(deviceHandle, Constants.WM_CAP_DRIVER_DISCONNECT, (IntPtr)deviceNumber, (IntPtr)0);
                User32.DestroyWindow(deviceHandle);
            }
            deviceHandle = new IntPtr(0);

        }
	
	
		  public static CaptureDevice GetDevice()
        {
       
            	CaptureDevice device = null;
                int capacity = 200;
                StringBuilder name = new StringBuilder(capacity);
                StringBuilder description = new StringBuilder(capacity);

                if (Avicap32.capGetDriverDescription(0, name, capacity, description, capacity).ToInt32() > 0)
                {
                   device = new CaptureDevice(0, name.ToString(), description.ToString());
                }
            

            return device;
        }
	
	}

}

