// sdk20-eth-zb client
// Authors: Vasiliy Pinkevich <vass258@yandex.ru>, Kirill Bulygin <kirill.bulygin@gmail.com>

import java.io.*;
import java.net.*;
import java.util.*;

public class EZBClient {
    public static void main(String[] args) throws IOException {
		if(args.length != 1) { // check server ip-address
			System.out.printf("Usage: java EZBClient server_ip_address\n");
			System.exit(1);
		}

		InetAddress ip = null;
		try {
			ip = InetAddress.getByName(args[0]);
		} catch(Exception e) {
			System.err.println(e);
			System.exit(1);
		}


		InputStreamReader isrIn = new InputStreamReader(System.in);
		BufferedReader brIn = new BufferedReader(isrIn);

		while(true) {
			// get slave address
			int port;

			try {
				System.out.printf("> ZB address: ");
				String s = brIn.readLine();
				if(s == null) {
					System.out.printf("\n");
					break;
				}
				port = Integer.parseInt(s);
			} catch(Exception e) {
				System.err.println(e);
				continue;
			}

			byte[] outbuf, inbuf;
			
			// create socket at port 50000
			DatagramSocket socket = new DatagramSocket();
			try {
				socket = new DatagramSocket();
			} catch(Exception e) {
				System.err.println(e);
				break;
			}
			socket.setSoTimeout(5000);

			while(true) { // chat
				System.out.printf("%d> ", port);
				String message = brIn.readLine();
				
				if(message == null) {
					System.out.printf("\n");
					break;
				}
				System.out.printf("[%d input chars]\n", message.length());
				
				outbuf = message.getBytes();
				
				DatagramPacket packet = new DatagramPacket(outbuf, outbuf.length, ip, port);
				try {
					socket.send(packet);
					packet = new DatagramPacket(outbuf, outbuf.length);
					socket.receive(packet);
				} catch(SocketTimeoutException ste) {
					System.err.println("[!] Server is lost");
					continue;
				} catch(Exception e) {
					System.err.println(e);
					continue;
				}

				// display response
				String received = new String(packet.getData(), 0, packet.getLength());
				System.out.println(received);
				/* // print reply in hex
				inbuf = packet.getData();
				System.out.printf("# Reply in hex: ");
				for(char i = 0; i < buf.length; i++)
					System.out.printf("%02X ", buf[i]);
				System.out.printf("\n");
				*/
			}
			socket.close();
		}
	}
}
