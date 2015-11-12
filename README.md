# LightjamsSpout
COM wrapper for Spout making it usable in c# and any .Net in general

Spout is a realtime video sharing framework for Windows. This COM object lets you easily create a Spout receiver in c#.

The SpoutSDK (https://github.com/leadedge/Spout2) is included and built as a static library.


Usage

You can create multiple instances of the LightjamsSpoutReceiver if you want multiple receivers.


Enumerate senders


```
try
{
    var receiver = new LightjamsSpoutLib.LightjamsSpoutReceiver();
    int nbSenders = receiver.NbSenders();
    for (int t = 0; t < nbSenders; ++t)
    {
        string name;
        int w, h;

        receiver.GetSenderInfo(t, out name, out w, out h);

        // add sender to your list
    }
    
}
catch(System.Runtime.InteropServices.COMException e)
{
	...
}
```

Receive images

You need to receive images always on the same thread. The Connect method can be called on a different thread, but once you start calling ReceiveImage, always use the same thread.

```
// initialization

try
{
	m_receiver =  new LightjamsSpoutLib.LightjamsSpoutReceiver();

	// the senderName as retrieved when enumerating senders or "" to use the active sender
	m_receiver.Connect(senderName, out m_width, out m_height);
	m_bitmap = new Bitmap(m_width, m_height, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
	m_buffer = new byte[m_width * m_height * 3];	
}
catch(System.Runtime.InteropServices.COMException e)
{
	...
}

...

// in your render loop

try
{
	m_receiver.ReceiveImage(m_buffer, LightjamsSpoutLib.EPixelFormat.BGR);
	var data = m_bitmap.LockBits(new Rectangle(0, 0, m_width, m_height), System.Drawing.Imaging.ImageLockMode.WriteOnly, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
	System.Runtime.InteropServices.Marshal.Copy(m_buffer, 0, data.Scan0, m_buffer.Length);
	m_bitmap.UnlockBits(data);
}
catch(System.Runtime.InteropServices.COMException e)
{
	...
}
```

Handle exceptions

The COM object will throw exceptions if something is wrong. For example, if the sender is gone, you'll get the string ERROR_SENDER_NOT_FOUND. If the image's size changed, you'll get the string SIZE_CHANGED. In most cases, for robustness and simplicity sake, you should destruct the COM object, create a new object and connect again. 