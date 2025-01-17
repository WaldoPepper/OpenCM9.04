/*******************************************************************************
* Copyright 2016 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/* Authors: Taehun Lim (Darby) */

/*******************************************************************************
* WARNING
* DYNAMIXEL Workbench library is deprecated and will not be updated after 2020.
* Please use DYNAMIXEL2Arduino or DYNAMIXEL SDK instead.
* https://emanual.robotis.com/docs/en/parts/controller/opencm904/#library-api
*******************************************************************************/

#include <DynamixelWorkbench.h>

#if defined(__OPENCM904__)
  #define DEVICE_NAME "3" //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP
#elif defined(__OPENCR__)
  #define DEVICE_NAME ""
#endif   

#define BAUDRATE  57600
#define DXL_ID_1  1
#define DXL_ID_2  2

DynamixelWorkbench dxl_wb;

uint8_t dxl_id[2] = {DXL_ID_1, DXL_ID_2};
int32_t goal_position[2] = {0, 1023};

const uint8_t handler_index = 0;

void setup() 
{
  Serial.begin(57600);
  // while(!Serial); // Wait for Opening Serial Monitor

  const char *log;
  bool result = false;

  uint16_t model_number = 0;

  result = dxl_wb.init(DEVICE_NAME, BAUDRATE, &log);
  if (result == false)
  {
    Serial.println(log);
    Serial.println("Failed to init");
  }
  else
  {
    Serial.print("Succeeded to init : ");
    Serial.println(BAUDRATE);  
  }

  for (int cnt = 0; cnt < 2; cnt++)
  {
    result = dxl_wb.ping(dxl_id[cnt], &model_number, &log);
    if (result == false)
    {
      Serial.println(log);
      Serial.println("Failed to ping");
    }
    else
    {
      Serial.println("Succeeded to ping");
      Serial.print("id : ");
      Serial.print(dxl_id[cnt]);
      Serial.print(" model_number : ");
      Serial.println(model_number);
    }

    result = dxl_wb.jointMode(dxl_id[cnt], 0, 0, &log);
    if (result == false)
    {
      Serial.println(log);
      Serial.println("Failed to change joint mode");
    }
    else
    {
      Serial.println("Succeed to change joint mode");
    }
  } 

  result = dxl_wb.addSyncWriteHandler(dxl_id[0], "Goal_Position", &log);
  if (result == false)
  {
    Serial.println(log);
    Serial.println("Failed to add sync write handler");
  }

  result = dxl_wb.addSyncReadHandler(dxl_id[0], "Present_Position", &log);
  if (result == false)
  {
    Serial.println(log);
    Serial.println("Failed to add sync read handler");
  }
}

void loop() 
{  
  const char *log;
  bool result = false;

  int32_t present_position[2] = {0, 0};

  result = dxl_wb.syncWrite(handler_index, &goal_position[0], &log);
  if (result == false)
  {
    Serial.println(log);
    Serial.println("Failed to sync write position");
  }

  do
  {
    result = dxl_wb.syncRead(handler_index, &log);
    if (result == false)
    {
      Serial.println(log);
      Serial.println("Failed to sync read position");
    }

    result = dxl_wb.getSyncReadData(handler_index, &present_position[0], &log);
    if (result == false)
    {
      Serial.println(log);
    }
    else
    {
      Serial.print("[ID ");
      Serial.print(dxl_id[0]);
      Serial.print(" ]");
      Serial.print(" Goal Position : ");
      Serial.print(goal_position[0]);
      Serial.print(" Present Position : ");
      Serial.print(present_position[0]);
      Serial.print(" [ID ");
      Serial.print(dxl_id[1]);
      Serial.print(" ]");
      Serial.print(" Goal Position : ");
      Serial.print(goal_position[1]);
      Serial.print(" Present Position : ");
      Serial.println(present_position[1]);
    }

  }while(abs(goal_position[0] - present_position[0]) > 15 && 
        abs(goal_position[1] - present_position[1]) > 15);

  swap(goal_position);
}

void swap(int32_t *array)
{
  int32_t tmp = array[0];
  array[0] = array[1];
  array[1] = tmp;
}