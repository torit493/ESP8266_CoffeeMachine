//Webpage stuff:########################################################
//// Set your Static IP address, gateway and subnet (see control panel -> network status -> wifi details...)
//IPAddress local_IP(192, 168, 0, 22);
//IPAddress gateway(192, 168, 0, 1);
//IPAddress subnet(255, 255, 255, 0);

// These things are determining the look of your page, but wont write anything
String head           = "<html>"                                      //the start of a html page with grey background for the application.
                        "<head>"
                        "<style>"
                        "body{background-color:#444444; color:white;}"// This gives background color of webpage
                        "a:link{color:white;}"                        // the color of links before pressed
                        "a:visited{color:yellow;}"                    // the color of links after pressed
                        "</style>"
                        "</head>"
                        "<body>";
String christmas_head = "<html>"                                      //the start of a html page with grey background for the application.
                        "<head>"
                        "<style>"
                        "body{background-color:#444444; color:#47ab35;}"
                        "a:link{color:#fffb5e;}"
                        "a:visited{color:#e03939;}"
                        "</style>"
                        "</head>"
                        "<body>";
String ending         = "</body> </html>";                            // ends the document by closing the body and html parts

String startup        = head + 
                        "This is the site controlling my thermostat. For info check <a href=\"/info\">/info</a>." 
                        + ending;
//######################################################################