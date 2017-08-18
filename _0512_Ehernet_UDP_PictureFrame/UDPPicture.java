/*

 Java UDP picture sender

 This programm reads an image and sends it via UDP.
 The protocol uses two commands: set position and draw pixel.	

 18. August 2017
 by ChrsiMicro


 How to use java:
 1. install java jdk
 2. compile this file ( UDPSender.java )
    
    javac UDPSender.java

 3. run it
    java UDPSender 

*/
import java.io.*;
import java.net.*;
import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;


class UDPPicture
{
	public static void delay_ms(int t_ms)
	{
	    try 
	    {
		Thread.sleep(t_ms);           
	    } catch(InterruptedException ex) 
	    {
		Thread.currentThread().interrupt();
	    }
	}

	public static Color[][] loadPixelsFromImage(File file) throws IOException 
	{
		BufferedImage image = ImageIO.read(file);
		Color[][] colors = new Color[image.getWidth()][image.getHeight()];

		for (int x = 0; x < image.getWidth(); x++) 
		{
			for (int y = 0; y < image.getHeight(); y++) 
			{
				colors[x][y] = new Color(image.getRGB(x, y));
		    	}
		}

		return colors;
	}

	public static void main(String args[]) throws Exception
	{
		Color[][] colors = loadPixelsFromImage(new File("image.png"));

		String  destinationIpAddress       = "192.168.178.177";
		int     destinationIpPort          = 8888;

		int delayTimebetweenUdpPackets_ms  = 10;
		int arduinoAcceptableUdpPacketLength = 24; // yes, the arduino allows only 24 byte payload

		BufferedReader inFromUser =   new BufferedReader(new InputStreamReader(System.in));
		DatagramSocket clientSocket = new DatagramSocket();

		InetAddress IPAddress = InetAddress.getByName( destinationIpAddress );

		byte[] sendData = new byte[arduinoAcceptableUdpPacketLength];

		int maxX=colors.length;
		System.out.println(maxX);
		int maxY=colors[0].length;
		System.out.println(maxY);

		byte COMMAND_DRAWPIXEL   = 0;
		byte COMMAND_SETPOSITION = 1;

		int posX = Integer.MAX_VALUE; // dirty hack to start positioning
		int posY = -1; // first action will increment this by one
		do
		{
			if( posX >= maxX ) // if next line, send a positioning command
			{
				posX=0;
				posY++;
			
				sendData[0] = COMMAND_SETPOSITION;

				sendData[2] = (byte)( posX     & 0xFF);
				sendData[3] = (byte)((posX>>8) & 0xFF);
				sendData[4] = (byte)( posY     & 0xFF);
				sendData[5] = (byte)((posY>>8) & 0xFF);

				DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, destinationIpPort);
				clientSocket.send(sendPacket);
				delay_ms(delayTimebetweenUdpPackets_ms);
			}

			sendData[0] = COMMAND_DRAWPIXEL;

			for(int k=2;k<arduinoAcceptableUdpPacketLength;k+=2)
			{
				if(posX<maxX && posY<maxY)
				{
					Color c=colors[posX][posY];
					int r = c.getRed()>>3;
					int g = c.getGreen()>>2;
					int b = c.getBlue()>>3;
					int rgb565 = (r << 11) | (g << 5) | b;

					sendData[k]   = (byte)(rgb565&0xFF);
					sendData[k+1] = (byte)((rgb565>>8)&0xFF);
				}else
				{
					sendData[k]   = 0;
					sendData[k+1] = 0;
				}
				posX++;
			}

			DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, destinationIpPort);
			clientSocket.send(sendPacket);
			delay_ms(delayTimebetweenUdpPackets_ms);
			System.out.println(posX+" "+posY);

		}while(posY<maxY);
	}
}
/* 
   © ChrisMicro 2017.

   It is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   GuiPittix is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public License
   along with this software.  If not, see <http://www.gnu.org/licenses/>.

   web-site: https://github.com/ChrisMicro
*/
