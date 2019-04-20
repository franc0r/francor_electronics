#!/usr/bin/env python
import rospy
from std_msgs.msg import Float32

g_rx_timestamp = rospy.Time()
g_duty_cycle_m1 = 0.0
g_duty_cycle_m2 = 0.0
g_duty_cycle_m3 = 0.0
g_duty_cycle_m4 = 0.0

max_timeout = 5.0

def callback_M1(data):
  global g_rx_timestamp, g_duty_cycle_m1
  g_duty_cycle_m1 = float(data.data)
  g_rx_timestamp = rospy.Time.now()

def callback_M2(data):
  global g_rx_timestamp, g_duty_cycle_m2
  g_duty_cycle_m2 = float(data.data)
  g_rx_timestamp = rospy.Time.now()

def callback_M3(data):
  global g_rx_timestamp, g_duty_cycle_m3
  g_duty_cycle_m3 = float(data.data)
  g_rx_timestamp = rospy.Time.now()

def callback_M4(data):
  global g_rx_timestamp, g_duty_cycle_m4
  g_duty_cycle_m4 = float(data.data)
  g_rx_timestamp = rospy.Time.now()


if __name__ == '__main__':
  # Init ROS
  rospy.init_node('drive_300b_interface')

  # Register subscribers
  rospy.Subscriber('/drive/m1/speed', Float32, callback_M1)
  rospy.Subscriber('/drive/m2/speed', Float32, callback_M2)
  rospy.Subscriber('/drive/m3/speed', Float32, callback_M3)
  rospy.Subscriber('/drive/m4/speed', Float32, callback_M4)

  # Run loop
  rate = rospy.Rate(1)
  while not rospy.is_shutdown():
    # Calculate last message time
    delta_rx_time = rospy.Time.now() - g_rx_timestamp
    if(delta_rx_time.to_sec() < max_timeout):
      # Convert to fixed point
      duty_cycle_m1 = int(g_duty_cycle_m1 * 100.0)
      duty_cycle_m2 = int(g_duty_cycle_m2 * 100.0)
      duty_cycle_m3 = int(g_duty_cycle_m3 * 100.0)
      duty_cycle_m4 = int(g_duty_cycle_m4 * 100.0)

      # Generate serial string
      data = ("SPD %+.5i %+.5i %+.5i %+.5i\r\n" % (duty_cycle_m1, duty_cycle_m2, duty_cycle_m3, duty_cycle_m4))

    else:
      print("Timeout last message " + str(delta_rx_time.to_sec()) + " seconds ago")
    
    rate.sleep()

  rospy.spin()