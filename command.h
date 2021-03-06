/*
 * Part of Strimer Plus Arduino
 * Version: 2021.10.27
 * Author: Murat TAMCI
 * Web Site: www.themt.co
 * License: Read LICENSE.txt
 */

String read_string;
char* cmd_cursor; // cursor
String cmd_key; // key
String cmd_value; // value
bool cmd_fail; 

void cmdWork (String cmd_key, String cmd_value);

void cmdProc () {
  if (Serial.available())  {
    char c = Serial.read();
    if (c == '\n' || c == '\0' || c == '\r') {
      Serial.read();
      //Serial.println(read_string);
  
      cmd_cursor = read_string.c_str();
      
      while (*cmd_cursor != '\0') {
 
        while (*cmd_cursor == '\t' || *cmd_cursor == ' ' && *cmd_cursor != '\0') { cmd_cursor++; } // skip all whitespace
  
        cmd_key = "";
        cmd_value = "";
        
        // i found key
        
        if (*(cmd_cursor-1) != '\\' && *cmd_cursor == '-') {
  
          cmd_fail = false;
  
          if (*cmd_cursor != '\0') cmd_cursor++; // skip "-" character
          
          while (*cmd_cursor != '\t' && *cmd_cursor != ' ' && *cmd_cursor != '\0') {
            cmd_key += *cmd_cursor; cmd_cursor++;
          }
  
          if (cmd_key.length() == 0) {
            cmd_fail = true;
            Serial.println("FAIL key is empty");
          }
  
          if (*cmd_cursor != '\0') cmd_cursor++; // skip whitespace character
  
          // i found value
  
          while (*cmd_cursor != '\0') {
            if (*cmd_cursor == '\\') {
              cmd_cursor++;
            } else if (*(cmd_cursor+1) == '-') break;
            cmd_value += *cmd_cursor; cmd_cursor++;
          }
  
          if (cmd_value.length() == 0) {
            cmd_fail = true;
            Serial.println("FAIL value is empty");
          }
  
          if (!cmd_fail) cmdWork (cmd_key, cmd_value);
  
          //Serial.print ("key: "); Serial.println (cmd_key);
          //Serial.print ("val: "); Serial.println (cmd_value);
        } else {
          if (*cmd_cursor != '\0') cmd_cursor++;
        }
      }
      
      read_string=""; //clears variable for new input      
     }  
    else {     
      read_string += c; //makes the string read_string
    }
  }
}
