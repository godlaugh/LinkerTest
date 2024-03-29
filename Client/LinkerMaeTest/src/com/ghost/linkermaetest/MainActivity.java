package com.ghost.linkermaetest;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import android.annotation.SuppressLint;
import android.os.Bundle;
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

		public PlaceholderFragment()
		{
		}
		
		public void test(final View rootView, final Boolean useMae)
		{
			final TextView textView;
			if (useMae)
			{
				textView = (TextView)rootView.findViewById(R.id.text_mae);
			}
			else 
			{
				textView = (TextView)rootView.findViewById(R.id.text_normal);
			}
			textView.setText("testing...");
			
			Thread thread = new Thread(new Runnable() {
				
				@Override
				public void run()
				{
					// TODO Auto-generated method stub
					
					String ip = "115.182.64.21";
					int lport = 7887;
					
					if (useMae)
					{
						String lPortStr =  LinkerMaeAdaptor.LinkerMaeAdaptorInit(textView.getContext().getApplicationContext(), "user1",ip, String.format("%d", lport) );
						System.out.println("mae port is " + lPortStr);
						if ( -1 == lport )
						{
							rootView.getHandler().post(new Runnable() {
								
								@Override
								public void run()
								{
									// TODO Auto-generated method stub
									textView.setText("mae port is -1");
								}
							});
						}
						lport = Integer.parseInt(lPortStr);
						ip = "127.0.0.1";
					}
					
					if (0 < lport)
					{
						DataInputStream in = null;
						DataOutputStream out = null;
						Socket socket = null;
						try
						{
							socket = new Socket(ip, lport);
							
							in = new DataInputStream(socket.getInputStream());
							out = new DataOutputStream(socket.getOutputStream());
							
							int bufferSize = 1024*128;
							byte[] sendData = new byte[bufferSize];
							for (int i = 0; i < bufferSize; i++)
							{
								sendData[i] = (byte) ('A'+i%26);
							}
							byte[] receiveBuf = new byte[bufferSize];
							
							final long millsBegin = System.currentTimeMillis();
							for (int i = 0; i < 30; i++) 
							{
								int totalLen = 0;
								for (int j = 0; j < 8; j++)
								{
									out.write(sendData);
									int recvLen = 0;
									while (bufferSize > recvLen)
									{
										int len = in.read(receiveBuf, recvLen, bufferSize-recvLen);
										if (0 < len)
										{
											recvLen += len;
//											final String recvString = new String(receiveBuf, 0, len);
//											final int no = i+1;
//											System.out.print("["+no+"]recv len:" + len);
//											rootView.getHandler().post(new Runnable() {
//												
//												@Override
//												public void run()
//												{
//													// TODO Auto-generated method stub
//													textView.setText("["+no+"]recv len:"+len);
//												}
//											});
										}
										else 
										{
											final int fRecvLen = len;
											System.out.print(String.format("socket error! len=%d", fRecvLen));
											rootView.getHandler().post(new Runnable() {
													
												@Override
												public void run()
												{
													// TODO Auto-generated method stub
													textView.setText(String.format("socket error! len=%d", fRecvLen));
												}
											});
											break;
										}
									}
									totalLen += recvLen;
								}
								final int no = i+1;
								final int fTotalLen = totalLen;
								System.out.print("["+no+"]recv len:" + fTotalLen);
								rootView.getHandler().post(new Runnable() {
									
									@Override
									public void run()
									{
										// TODO Auto-generated method stub
										textView.setText("["+no+"]recv len:"+fTotalLen);
									}
								});
							}
							final long millsEnd = System.currentTimeMillis();
							rootView.getHandler().post(new Runnable() {
								
								@Override
								public void run()
								{
									// TODO Auto-generated method stub
									textView.setText(String.format("duration: %d mills", millsEnd-millsBegin));
								}
							});
						}
						catch (UnknownHostException e)
						{
							// TODO Auto-generated catch block
							e.printStackTrace();
							final String msg = e.getMessage();
							rootView.getHandler().post(new Runnable() {
								
								@Override
								public void run()
								{
									// TODO Auto-generated method stub
									textView.setText(msg);
								}
							});
						}
						catch (IOException e)
						{
							// TODO Auto-generated catch block
							e.printStackTrace();
							final String msg = e.getMessage();
							rootView.getHandler().post(new Runnable() {
								
								@Override
								public void run()
								{
									// TODO Auto-generated method stub
									textView.setText(msg);
								}
							});
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
						
						if (null != socket)
						{
							try
							{
								socket.close();
							}
							catch (IOException e)
							{
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
							socket = null;
						}
					}
					
					if (useMae)
					{
						LinkerMaeAdaptor.LinkerMaeAdaptorUninit();
					}
					
					rootView.getHandler().post(new Runnable() {
						
						@Override
						public void run()
						{
							// TODO Auto-generated method stub
							rootView.findViewById(R.id.button_normal).setEnabled(true);
							rootView.findViewById(R.id.button_mae).setEnabled(true);
						}
					});
				}
			});
			thread.start();
		}

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
				Bundle savedInstanceState)
		{
			final View rootView = inflater.inflate(R.layout.fragment_main, container,
					false);
			rootView.findViewById(R.id.button_normal).setOnClickListener(new View.OnClickListener() {
				
				@Override
				public void onClick(View v)
				{
					// TODO Auto-generated method stub
					v.setEnabled(false);
					rootView.findViewById(R.id.button_mae).setEnabled(false);
					test(rootView, false);
				}
			});
			rootView.findViewById(R.id.button_mae).setOnClickListener(new View.OnClickListener() {
				
				@Override
				public void onClick(View v)
				{
					// TODO Auto-generated method stub
					v.setEnabled(false);
					rootView.findViewById(R.id.button_normal).setEnabled(false);
					test(rootView, true);
				}
			});
			
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
