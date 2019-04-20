#!/usr/bin/env python
import rospy
import sys
import serial
from std_msgs.msg import Float32

g_rx_timestamp_m1 = rospy.Time()
g_rx_timestamp_m2 = rospy.Time()
g_rx_timestamp_m3 = rospy.Time()
g_rx_timestamp_m4 = rospy.Time()

g_duty_cycle_m1 = 0.0
g_duty_cycle_m2 = 0.0
g_duty_cycle_m3 = 0.0
g_duty_cycle_m4 = 0.0

max_timeout = 0.1

def callback_M1(data):
  global g_rx_timestamp_m1, g_duty_cycle_m1
  g_duty_cycle_m1 = float(data.data)
  g_rx_timestamp_m1 = rospy.Time.now()

def callback_M2(data):
  global g_rx_timestamp_m2, g_duty_cycle_m2
  g_duty_cycle_m2 = float(data.data)
  g_rx_timestamp_m2 = rospy.Time.now()

def callback_M3(data):
  global g_rx_timestamp_m3, g_duty_cycle_m3
  g_duty_cycle_m3 = float(data.data)
  g_rx_timestamp_m3 = rospy.Time.now()

def callback_M4(data):
  global g_rx_timestamp_m4, g_duty_cycle_m4
  g_duty_cycle_m4 = float(data.data)
  g_rx_timestamp_m4 = rospy.Time.now()


if __name__ == '__main__':
  # Init ROS
  rospy.init_node('bld300b_interface', anonymous=False, log_level=rospy.INFO)

  # Init variables
  g_rx_timestamp_m1 = rospy.Time.now()
  g_rx_timestamp_m2 = rospy.Time.now()
  g_rx_timestamp_m3 = rospy.Time.now()
  g_rx_timestamp_m4 = rospy.Time.now()

  serial_device = rospy.get_param('~serial_device', '/dev/ttyACM0')
  max_timeout = rospy.get_param('~timeout', 0.1)

  rospy.loginfo("Serial-Device: " + serial_device)
  rospy.loginfo("Timeout: " + str(max_timeout))


  # Open serial device
  try:
    nucleo_serial = serial.Serial(serial_device, 115200)
  except Exception as e:
    rospy.logerr("Failed to open serial device : " + serial_device);
    sys.exit(-1)

  # Register subscribers
  rospy.Subscriber('/drive/m1/speed', Float32, callback_M1)
  rospy.Subscriber('/drive/m2/speed', Float32, callback_M2)
  rospy.Subscriber('/drive/m3/speed', Float32, callback_M3)
  rospy.Subscriber('/drive/m4/speed', Float32, callback_M4)

  # Run loop
  rate = rospy.Rate(20)
  while not rospy.is_shutdown():
    # Calculate last message timestamps
    delta_rx_time_m1 = rospy.Time.now() - g_rx_timestamp_m1
    delta_rx_time_m2 = rospy.Time.now() - g_rx_timestamp_m2
    delta_rx_time_m3 = rospy.Time.now() - g_rx_timestamp_m3
    delta_rx_time_m4 = rospy.Time.now() - g_rx_timestamp_m4

    # Check if last message for every individual drive is to long ago
    # set speed to 0 after timeout
    if(delta_rx_time_m1.to_sec() > max_timeout):
      g_duty_cycle_m1 = 0.0
      rospy.logwarn("Timeout M1")     

    if(delta_rx_time_m2.to_sec() > max_timeout):
      g_duty_cycle_m2 = 0.0
      rospy.logwarn("Timeout M2")  

    if(delta_rx_time_m3.to_sec() > max_timeout):
      g_duty_cycle_m3 = 0.0
      rospy.logwarn("Timeout M3")  

    if(delta_rx_time_m4.to_sec() > max_timeout):
      g_duty_cycle_m4 = 0.0
      rospy.logwarn("Timeout M4")  

    # Convert to fixed point
    duty_cycle_m1 = int(g_duty_cycle_m1 * 100.0)
    duty_cycle_m2 = int(g_duty_cycle_m2 * 100.0)
    duty_cycle_m3 = int(g_duty_cycle_m3 * 100.0)
    duty_cycle_m4 = int(g_duty_cycle_m4 * 100.0)

    # Generate serial string
    data = ("SPD %+.5i %+.5i %+.5i %+.5i\r\n" % (duty_cycle_m1, duty_cycle_m2, duty_cycle_m3, duty_cycle_m4))
    
    # Send data
    try:
      nucleo_serial.write(data)
    except Exception as ed:
      rospy.logerr("Failed to send data to nucleo board!")
      sys.exit(-1)
    
    rate.sleep()

  rospy.spin()