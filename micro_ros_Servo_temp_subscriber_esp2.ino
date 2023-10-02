#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <ESP32Servo.h>         // 19
#include <DHT.h>           // temp - a/m

// #include <std_msgs/msg/int32.h>
#include <std_msgs/msg/int8.h>   // 1/9

Servo myservo_esp2;     // object   // 20
DHT dht_esp2(15, DHT11);  // temp - b

int servoPin_esp2 = 13;             // 21
#define TEMP_PIN_ESP2 15    // temp - c

// rcl_subscription_t subscriber;
rcl_subscription_t LEDs_subscriber_esp2;  // 2/9

rcl_subscription_t servo_subscriber_esp2;  // 22

rcl_publisher_t temp_publisher_esp2;  // temp - d

// std_msgs__msg__Int32 msg;
std_msgs__msg__Int8 LEDs_msg_esp2;     // 3/9

std_msgs__msg__Int8 servo_msg_esp2;    // 23  

std_msgs__msg__Int8 temp_msg_esp2;     // temp - e

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

// #define LED_PIN 2
#define GRN_LED_PIN_ESP2 18   // 10
#define RED_LED_PIN_ESP2 19    // 11

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

int8_t get_temp_esp2();   // temp - k

void error_loop(){
  while(1){
    // digitalWrite(LED_PIN, !digitalRead(LED_PIN));    // 18
    delay(100);
  }
}

int limitToMaxValue(int value, int maxLimit) {         // 25 - start
  if (value > maxLimit){
    return maxLimit;
  } else {
    return value;
  }
}                                                      // 25 - end

// void subscription_callback(const void * msgin)
// {  
//   const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
//   digitalWrite(LED_PIN, (msg->data == 0) ? LOW : HIGH);  
// }

void timer_callback_esp2(rcl_timer_t * timer, int64_t last_call_time){    // temp - i
  RCLC_UNUSED(last_call_time);
  if(timer != NULL){
    int8_t temp = get_temp_esp2();
    temp_msg_esp2.data = temp;
    RCSOFTCHECK(rcl_publish(&temp_publisher_esp2, &temp_msg_esp2, NULL));
  }
}

void LEDs_subscription_callback_esp2(const void * msgin)                           // 4/9
{  
  const std_msgs__msg__Int8 * msg = (const std_msgs__msg__Int8 *)msgin;
  // digitalWrite(LED_PIN, (msg->data == 0) ? LOW : HIGH);  

  int8_t value = msg->data;           // 16

  switch (value) {                    // 17
    case 0:
      digitalWrite(GRN_LED_PIN_ESP2, LOW);
      digitalWrite(RED_LED_PIN_ESP2, LOW);
      break;
    case 1:
      digitalWrite(GRN_LED_PIN_ESP2, LOW);
      digitalWrite(RED_LED_PIN_ESP2, HIGH);
      break;
    case 2:
      digitalWrite(GRN_LED_PIN_ESP2, HIGH);
      digitalWrite(RED_LED_PIN_ESP2, LOW);
      break;
    case 3:
      digitalWrite(GRN_LED_PIN_ESP2, HIGH);
      digitalWrite(RED_LED_PIN_ESP2, HIGH);
      break;
    default:
      break;
  }
}

void servo_callback_esp2(const void* msgin){                                   // 24 - start
  const std_msgs__msg__Int8* msg = (const std_msgs__msg__Int8*)msgin;
  int8_t angle = msg->data;
  int servo_position_esp2;
  servo_position_esp2 = limitToMaxValue(angle, 40);
  myservo_esp2.write(servo_position_esp2);
}

// void temp_callback(const void* msgin){                                   // f
//   const std_msgs__msg__Int8* msg = (const std_msgs__msg__Int8*)msgin;
//   int8_t angle = msg->data;
//   int temp;
//   temp = dht.read();
// }

int8_t get_temp_esp2(){                   // temp - l
  // int temp = analogRead(TEMP_PIN);
  int temp_esp2 = dht_esp2.readTemperature();
  return static_cast<int8_t>(temp_esp2);
}


void setup() {
  set_microros_wifi_transports("siddhi", "siddhi123", "192.168.43.158", 8889);
  // set_microros_transports();
  
  // pinMode(LED_PIN, OUTPUT);
  // digitalWrite(LED_PIN, HIGH);

  pinMode(GRN_LED_PIN_ESP2, OUTPUT);     // 12
  digitalWrite(GRN_LED_PIN_ESP2, HIGH);  // 13

  pinMode(RED_LED_PIN_ESP2, OUTPUT);      // 14
  digitalWrite(RED_LED_PIN_ESP2, HIGH);   // 15

  pinMode(TEMP_PIN_ESP2, INPUT);   // temp - j 
  
  delay(2000);

  dht_esp2.begin();  // g
  delay(2000);  // h

  Serial.begin(115200);  // i

  // Allow allocation of all timers    // 21 - start
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo_esp2.setPeriodHertz(50);    // standard 50 hz servo
	myservo_esp2.attach(servoPin_esp2, 1000, 2000);   // 21 - end

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  // RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));
  RCCHECK(rclc_node_init_default(&node, "ESP32_Terminal_2", "", &support));            // 5/9
  // RCCHECK(rclc_node_init_default(&node, "esp32_terminal_2", "", &support)); 

  // create subscriber

  // LED Subscriber
  RCCHECK(rclc_subscription_init_default(
    // &subscriber,
    &LEDs_subscriber_esp2,                                                            // 6/9
    &node,
    // ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int8),                            // 7/9
    // "micro_ros_arduino_subscriber"
    "LEDs_esp2"));                                                                    // 8/9

  // Servo Subscriber
  RCCHECK(rclc_subscription_init_default(                  // 22 - start
    // &subscriber,
    &servo_subscriber_esp2,                                                            
    &node,
    // ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int8),                            
    // "micro_ros_arduino_subscriber"
    "/servo_esp2"));                                              // 22 - end                     

  // create timer
  const unsigned int timer_timeout = 100;      // temp - f - start
  RCCHECK(rclc_timer_init_default(                  
    &timer,                                                            
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback_esp2));
         
  // publisher
  RCCHECK(rclc_publisher_init_default(              // temp - m       
    // &subscriber,
    &temp_publisher_esp2,                                                            
    &node,
    // ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int8),                            
    // "micro_ros_arduino_subscriber"
    "temp_esp2"));                                              

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 4, &allocator));     // no. of threades = no. of subscribers = 1  // temp - h
  RCCHECK(rclc_executor_add_timer(&executor, &timer));    // temp - g 

  // RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA));
  RCCHECK(rclc_executor_add_subscription(&executor, &LEDs_subscriber_esp2, &LEDs_msg_esp2, &LEDs_subscription_callback_esp2, ON_NEW_DATA));   // 9/9
  RCCHECK(rclc_executor_add_subscription(&executor, &servo_subscriber_esp2, &servo_msg_esp2, &servo_callback_esp2, ON_NEW_DATA));             // 23         
}

void loop() {
  delay(100);
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}
