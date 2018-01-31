using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Threading;
namespace week3lab
{
    class Program
    {

        static String message = "Y11";
       // String inputString = "";

        public static void Main(string[] args)
        {
            SerialPort port = new SerialPort();
            port.PortName = "COM8";
            port.BaudRate = 9600;
            port.DataReceived += new SerialDataReceivedEventHandler(DataReceivedEventHandler);
            port.Open();
            //set 
            port.Write("+++");
               
            Thread.Sleep(1500);
            port.WriteLine("ATID 3511, CH C, CN");

           // Thread.Sleep(1100);
           // port.DiscardInBuffer();
          //  port.DataReceived += new SerialDataReceivedEventHandler(DataReceivedEventHandler);

            while (true)
            {

                Console.Write("< ");
                message = Console.ReadLine();
                port.WriteLine(message);

                // message = port.ReadLine();
                //Console.WriteLine(">" + message);


            }



        }

        private static void DataReceivedEventHandler(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            string indata = sp.ReadExisting();
            Console.WriteLine("Data Received: " + indata);

        }

    }



}

