 The goal of this project is to develop a computer vision system that can analyze video footage of various sport 
 events (e.g., basket, soccer), providing high-level information such as player detection and team 
 classification, recognizing team strategies and playing field boundaries. In more detail, the system to be 
 developed should be capable of: 
  # 1. Localizing using a bounding box all the players in the image inside the playing field; 
 #  2. Segmenting the playing field and players from the rest of the image; 
  # 3. Classifying all the detected players according to their corresponding team.
 As case studies for the development of the required system, various types of team sports are considered, like 
 soccer, basketball, volleyball, hockey, and rugby. The system to be developed should be robust to all such 
 different scenarios. In particular: 
 #  ● it should recognize all the players within the playfield and their corresponding team; in some scenes, one of the players might be dressed in a different color with respect to the other team members (e.g., goalkeeper) - in such case, you are allowed not to associate a team to that player; 
#  ● it should ignore any non-player person (e.g., referees, spectators) outside the playing field; 
 # ● it should segment different types of playing field, considering different geometries and colors.
