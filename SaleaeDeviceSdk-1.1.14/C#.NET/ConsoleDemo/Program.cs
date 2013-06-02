using System;
using System.Collections.Generic;
using System.Text;
using SaleaeDeviceSdkDotNet;

namespace SaleaeDeviceDotNetConsole
{
    class Program
    {
        static void Main(string[] args)
        {
            ConsoleDemo demo = new ConsoleDemo();
            demo.Run();
        }
    }

    public class ConsoleDemo
    {
        UInt32 mSampleRateHz = 4000000;
        MLogic mLogic;
        MLogic16 mLogic16;
        byte mWriteValue = 0;

        public ConsoleDemo()
        {
            MSaleaeDevices devices = new MSaleaeDevices();
            devices.OnLogicConnect += new MSaleaeDevices.OnLogicConnectDelegate(devices_LogicOnConnect);
            devices.OnLogic16Connect += new MSaleaeDevices.OnLogic16ConnectDelegate(devices_Logic16OnConnect);
            devices.OnDisconnect += new MSaleaeDevices.OnDisconnectDelegate(devices_OnDisconnect);
            devices.BeginConnect();
            Console.WriteLine("Logic is currently set up to read and write at {0} Hz.  You can change this in the code.", mSampleRateHz);
        }

        public void Run()
        {
            while (true)
            {
                Console.WriteLine();
                Console.WriteLine();
                Console.WriteLine( "You can type read, write, readbyte, writebyte, setsamplerate [number], stop, or exit." );
                Console.WriteLine( "(r, w, rb, wb, ssr ###, s, and e for short)" );
                if (mLogic16 != null)
                {
                    Console.WriteLine("Logic16 does not support readbyte, writebyte, or write");
                    Console.WriteLine("You can also type setchannels [1,2,3..] and set5volts [true/false]");
                    Console.WriteLine("(sc and s5v for short)");
                }
                Console.WriteLine();
                Console.WriteLine();

                string command = Console.ReadLine();


		        if( command == "exit" || command == "e" )
			        break;

		        if( command == "" )
			        continue;

		        if( (mLogic == null) && (mLogic16 == null) )
		        {
			        Console.WriteLine( "Sorry, no devices are connected." );
			        continue;
		        } 

		        if( command == "stop" || command == "s" )
		        {
                    if (mLogic != null)
                    {
                        if (mLogic.IsStreaming() == false)
                            Console.WriteLine("Sorry, the device is not currently streaming.");
                        else
                            mLogic.Stop();
                    }
                    if (mLogic16 != null)
                    {
                        if (mLogic16.IsStreaming() == false)
                            Console.WriteLine("Sorry, the device is not currently streaming.");
                        else
                            mLogic16.Stop();
                    }

			        continue;
		        }
                if (mLogic != null)
                {
                    if (mLogic.IsStreaming() == true)
                    {
                        Console.WriteLine("Sorry, Logic is already streaming.");
                        continue;
                    }
                }
                if (mLogic16 != null)
                {
                    if (mLogic16.IsStreaming() == true)
                    {
                        Console.WriteLine("Sorry, Logic16 is already streaming.");
                        continue;
                    }
                }

		        if( command == "read" || command == "r" )
		        {
                    if (mLogic != null)
                        mLogic.ReadStart();
                    if (mLogic16 != null)
                        mLogic16.ReadStart();
		        }
		        else
		        if( command == "write" || command == "w" )
		        {
                    if (mLogic != null)
			            mLogic.WriteStart();
                    if (mLogic16 != null)
                        Console.WriteLine("Not supported by Logic16");
		        }
		        else
		        if( command == "readbyte" || command == "rb" )
		        {
                    if (mLogic != null)
                        Console.WriteLine( "Got value 0x{0:X}", mLogic.GetInput() );
                    if (mLogic16 != null)
                        Console.WriteLine("Not supported by Logic16");
		        }
		        else
		        if( command == "writebyte" || command == "wb" )
		        {
                    if (mLogic != null)
                    {
                        mLogic.SetOutput(mWriteValue);
                        Console.WriteLine("Logic is now outputting 0x{0:X}", mWriteValue);
                        mWriteValue++;
                    }
                    if (mLogic16 != null)
                        Console.WriteLine("Not supported by Logic16");
		        }
                else
                if ( command.StartsWith("setsamplerate") || command.StartsWith("ssr") )
                {
                    List<uint> sample_rates = new List<uint>();
                    if (mLogic != null)
                        sample_rates = mLogic.GetSupportedSampleRates();
                    if (mLogic16 != null)
                        sample_rates = mLogic16.GetSupportedSampleRates();

                    if (command.IndexOf(" ") == -1)
                    {
                        Console.WriteLine("Avalable sample rates: ");
                        for (int i = 0; i < sample_rates.Count; ++i)
                            Console.WriteLine(sample_rates[i].ToString());
                        continue;
                    }
                    
                    uint new_rate;
                    string num_string = command.Substring(command.IndexOf(" ") + 1);
                    if (uint.TryParse(num_string, out new_rate))
                    {
                        
                        if (sample_rates.IndexOf(new_rate) != -1)
                        {
                            if (mLogic != null)
                                mLogic.SampleRateHz = new_rate;
                            if (mLogic16 != null)
                                mLogic16.SampleRateHz = new_rate;
                            Console.WriteLine("New Sample Rate: {0}", new_rate);
                        }
                        else
                        {
                           //uint sugested_rate = sample_rates
                            Console.WriteLine("Sample Rate Not supported. Avalable rates: ");
                            for (int i = 0; i < sample_rates.Count; ++i)
                                Console.WriteLine(sample_rates[i].ToString());
                        }
                    }
                    else
                    {
                        Console.WriteLine("Avalable sample rates: ");
                        for (int i = 0; i < sample_rates.Count; ++i)
                            Console.WriteLine(sample_rates[i].ToString());
                        continue;
                    }
                }
                if (command.StartsWith("setchannels") || command.StartsWith("sc"))
                {
                    if (mLogic16 == null)
                        continue;

                    if (command.IndexOf(" ") != -1)
                    {
                        command = command.Substring(command.IndexOf(" ") + 1);
                    }
                    else
                    {
                        Console.WriteLine("Active Channels: " + string.Join(", ", Array.ConvertAll<uint, String>( mLogic16.CaptureChannels.ToArray(), new Converter<uint, string>( Convert.ToString ))));
                        continue;
                    }
                    String[] strings = command.Split(',');
                    List<uint> channels = new List<uint>();
                    foreach (String str in strings)
                    {
                        uint channel;
                        if (uint.TryParse(str.Trim(), out channel))
                            channels.Add(channel);
                    }
                    if (channels.Count > 0)
                        mLogic16.CaptureChannels = channels;
                    else
                        Console.WriteLine("Invalid Input, please type: setchannels 1, 2, 3, 4");
                }
                if (command.StartsWith("set5volts") || command.StartsWith("s5v"))
                {
                    if (mLogic16 == null)
                        continue;
                    if (command.IndexOf(" ") != -1)
                    {
                        command = command.Substring(command.IndexOf(" ") + 1);
                    }
                    else
                    {
                        if (mLogic16.Use5Volts == true)
                            Console.WriteLine("I/O voltage: 3.6V to 5.0V");
                        else
                            Console.WriteLine("I/O voltage: 1.8V to 3.6V");
                        continue;
                    }
                    if (command.ToLower().Contains("true"))
                        mLogic16.Use5Volts = true;
                    else if (command.ToLower().Contains("false"))
                        mLogic16.Use5Volts = false;
                    else
                        Console.WriteLine("Invalid input please type: set5volts true or set5volts false");
                }
            }
        }

        void devices_OnDisconnect(ulong device_id)
        {
            Console.WriteLine("Device with id {0} disconnected.", device_id);
            if (mLogic != null)
                mLogic = null;
            if (mLogic16 != null)
                mLogic16 = null;
        }

        void devices_LogicOnConnect(ulong device_id, MLogic logic)
        {
            Console.WriteLine("Logic with id {0} connected.", device_id);
            
            mLogic = logic;
            mLogic.OnReadData += new MLogic.OnReadDataDelegate(mLogic_OnReadData);
            mLogic.OnWriteData += new MLogic.OnWriteDataDelegate(mLogic_OnWriteData);
            mLogic.OnError += new MLogic.OnErrorDelegate(mLogic_OnError);
            mLogic.SampleRateHz = mSampleRateHz;
        }

        void devices_Logic16OnConnect(ulong device_id, MLogic16 logic_16)
        {
            Console.WriteLine("Logic16 with id {0} connected.", device_id);

            mLogic16 = logic_16;
            logic_16.OnReadData += new MLogic16.OnReadDataDelegate(mLogic16_OnReadData);
            logic_16.OnError += new MLogic16.OnErrorDelegate(mLogic_OnError);
            logic_16.SampleRateHz = mSampleRateHz;
        }

        void mLogic_OnReadData(ulong device_id, byte[] data)
        {
            Console.WriteLine("Logic: Read {0} bytes, starting with 0x{1:X}", data.Length, (ushort)data[0]);
        }
        void mLogic16_OnReadData(ulong device_id, byte[] data)
        {
            Console.WriteLine("Logic16: Read {0} bytes, starting with 0x{1:X}", data.Length, data[0]);
        }

        void mLogic_OnWriteData(ulong device_id, byte[] data)
        {
            for (int i = 0; i < data.Length; i++)
            {
                data[i] = mWriteValue;
                mWriteValue++;
            }

            Console.WriteLine("Wrote {0} bytes of data", data.Length );
        }

        void mLogic_OnError(ulong device_id)
        {
            Console.WriteLine( "Logic Reported an Error.  This probably means that Logic could not keep up at the given data rate, or was disconnected. You can re-start the capture automatically, if your application can tolerate gaps in the data.");
        }
    }

}
