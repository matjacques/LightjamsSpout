# LightjamsSpout
# COM wrapper for Spout making it usable in c# and .Net in general.

Spout is a realtime video sharing framework for Windows. This COM object lets you easily create a Spout receiver in c#.

The SpoutSDK (https://github.com/leadedge/Spout2) is included and built as a static library.


# Usage

You can create multiple instances of the LightjamsSpoutReceiver if you want multiple receivers.


## Enumerate senders


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

## Create the OpenGL context

One context is needed per thread that will receive images with the receiver. If you don't already have a context, use the GLContext object like this:

```

// can instanciate the object in any thread
m_glContext = new LightjamsSpoutLib.GLContext();

...


// create the context in the same thread that will receive images
try
{    
    m_glContext.Create();    
}
catch(System.Runtime.InteropServices.COMException e)
{
	...
}
```

Release this context when closing your application or when you don't need the receivers anymore.



## Receive images

Always receive images on the same thread. The Connect method can be called on a different thread, but once you start calling ReceiveImage, always use the same thread. You can create as many receivers as you want by instanciating multiple LightjamsSpoutReceiver.

```
// initialization from any thread

try
{
	m_receiver =  new LightjamsSpoutLib.LightjamsSpoutReceiver();

	// the senderName as retrieved when enumerating senders or "" to use the active sender
	m_receiver.Connect(senderName, out m_width, out m_height);
	m_bitmap = new Bitmap(m_width, m_height, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
	
	const int bytesPerPixel = 3;	// RGB
    int stride = 4 * ((m_width * bytesPerPixel + 3) / 4);
    m_buffer = new byte[stride * m_height];	
}
catch(System.Runtime.InteropServices.COMException e)
{
	...
}

...

// in your render thread, you call this in a loop

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

Alternatively, you can pass a pointer to the bitmap data directly if you don't want to do anything with the bytes yourself:

```
try
{
	var data = m_bitmap.LockBits(new Rectangle(0, 0, m_width, m_height), System.Drawing.Imaging.ImageLockMode.WriteOnly, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
    m_receiver.ReceiveImageIntPtr((long)data.Scan0, LightjamsSpoutLib.EPixelFormat.BGR);                
    m_bitmap.UnlockBits(data);                
                
}
catch(System.Runtime.InteropServices.COMException e)
{
	...
}
```


## Handle exceptions

The COM object will throw exceptions if something is wrong. For example, if the sender is gone, you'll get the string ERROR_SENDER_NOT_FOUND. If the image's size changed, you'll get the string SIZE_CHANGED. In most cases, for robustness and simplicity sake, you should destruct the COM object, create a new object and connect again. 

## Deployment

The best way to deploy the COM object with your application is to add some info to your application manifest file in order to use LightjamsSpout as a registration-free COM. This way, the user installing your app doesn't need to be an administrator and there's no chance of conflict with another installed app using another version of LightjamsSpout.

Don't forget to bundle the right file version (32 or 64 bit) to fit your project.

A typical application manifest with proper dependency info looks like this:

```
<?xml version="1.0" encoding="utf-8"?>
<asmv1:assembly manifestVersion="1.0" xmlns="urn:schemas-microsoft-com:asm.v1" xmlns:asmv1="urn:schemas-microsoft-com:asm.v1" xmlns:asmv2="urn:schemas-microsoft-com:asm.v2" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <assemblyIdentity version="1.0.0.0" name="MyApplication.app"/>
  <trustInfo xmlns="urn:schemas-microsoft-com:asm.v2">
    <security>
      <requestedPrivileges xmlns="urn:schemas-microsoft-com:asm.v3">        
        <requestedExecutionLevel level="asInvoker" uiAccess="false" />
      </requestedPrivileges>
    </security>
  </trustInfo>

  <compatibility xmlns="urn:schemas-microsoft-com:compatibility.v1">
    <application>
      <!-- A list of all Windows versions that this application is designed to work with. 
      Windows will automatically select the most compatible environment.-->

      <!-- If your application is designed to work with Windows Vista, uncomment the following supportedOS node-->
      <supportedOS Id="{e2011457-1546-43c5-a5fe-008deee3d3f0}"></supportedOS>

      <!-- If your application is designed to work with Windows 7, uncomment the following supportedOS node-->
      <supportedOS Id="{35138b9a-5d96-4fbd-8e2d-a2440225f93a}"/>

      <!-- If your application is designed to work with Windows 8, uncomment the following supportedOS node-->
      <supportedOS Id="{4a2f28e3-53b9-4441-ba9c-d69d4a4a6e38}"></supportedOS>

      <!-- If your application is designed to work with Windows 8.1, uncomment the following supportedOS node-->
      <supportedOS Id="{1f676c76-80e1-4239-95bb-83d0f6d0da78}"/>

    </application>
  </compatibility>

  <dependency>
    <dependentAssembly>
      <assemblyIdentity type="win32"
                        name="LightjamsSpout"
                        version="1.0.0.9"                        
                        language="*"/>

    </dependentAssembly>

  </dependency>

  
</asmv1:assembly>

```