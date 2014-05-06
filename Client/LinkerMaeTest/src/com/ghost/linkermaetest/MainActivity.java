package com.ghost.linkermaetest;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.Fragment;
import android.support.v7.app.ActionBarActivity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.mae.sdk.LinkerMaeAdaptor;

public class MainActivity extends ActionBarActivity {

	@SuppressLint("NewApi")
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.activity_main);

		if (savedInstanceState == null)
		{
			getSupportFragmentManager().beginTransaction()
					.add(R.id.container, new PlaceholderFragment()).commit();
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{

		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings)
		{
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	/**
	 * A placeholder fragment containing a simple view.
	 */
	public static class PlaceholderFragment extends Fragment {
		
		private Thread thread_ = null;

		public PlaceholderFragment()
		{
		}

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
				Bundle savedInstanceState)
		{
			final View rootView = inflater.inflate(R.layout.fragment_main, container,
					false);
			
			if (null == thread_)
			{
				final Handler handler = new Handler();
				thread_ = new Thread(new Runnable() {
					private Socket socket_ = null;
					
					@Override
					public void run()
					{
						// TODO Auto-generated method stub
						DataInputStream in = null;
						DataOutputStream out = null;
						
						if (null == socket_)
						{
							String ip = "115.182.64.21";
							String port = "7887";
							String useri = "user1";
							
							String local_port = LinkerMaeAdaptor.LinkerMaeAdaptorInit( useri,ip, port );
							
							System.out.println("port is " + local_port);
							
							int lport = Integer.parseInt(local_port);
							
							if ( -1 != lport )
							{
								try
								{
//									socket_ = new Socket(ip, 7887);
									socket_ = new Socket("127.0.0.1", lport);
									
									in = new DataInputStream(socket_.getInputStream());
									out = new DataOutputStream(socket_.getOutputStream());
									
									for (int i = 0; i<10; i++) 
									{
										try
										{
											Thread.sleep(1000);
										}
										catch (InterruptedException e)
										{
											// TODO Auto-generated catch block
											e.printStackTrace();
										}
										
										out.writeBytes("hello:"+i);
										
										byte[] receiveBuf = new byte[2048];
									
										int len = in.read(receiveBuf);
										
										if (0 < len)
										{
											final String recvString = new String(receiveBuf, 0, len);
											System.out.print("recv:" + recvString);
											handler.post(new Runnable() {
												
												@Override
												public void run()
												{
													// TODO Auto-generated method stub
													((TextView)rootView.findViewById(R.id.text)).setText("recv:"+recvString);
												}
											});
										}
										else 
										{
											final int recvLen = len;
											System.out.print(String.format("socket error! len=%d", len));
handler.post(new Runnable() {
													
													@Override
													public void run()
													{
														// TODO Auto-generated method stub
													((TextView)rootView.findViewById(R.id.text)).setText(String.format("socket error! len=%d", recvLen));
												}
											});
										}
									}
								}
								catch (UnknownHostException e)
								{
									// TODO Auto-generated catch block
									e.printStackTrace();
								}
								catch (IOException e)
								{
									// TODO Auto-generated catch block
									e.printStackTrace();
								}
							}
						}
						
						if (null != in)
						{
							try
							{
								in.close();
							}
							catch (IOException e1)
							{
								// TODO Auto-generated catch block
								e1.printStackTrace();
							}
							in = null;
						}
						if (null != out)
						{
							try
							{
								out.close();
							}
							catch (IOException e1)
							{
								// TODO Auto-generated catch block
								e1.printStackTrace();
							}
							out = null;
						}
						
						if (null != socket_)
						{
							try
							{
								socket_.close();
							}
							catch (IOException e)
							{
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
							socket_ = null;

							LinkerMaeAdaptor.LinkerMaeAdaptorUninit();
						}
					}
				});
				thread_.start();
			}
			
			return rootView;
		}
		
		@Override
		public void onDestroyView()
		{
			// TODO Auto-generated method stub
			super.onDestroyView();
		}
	}

}
