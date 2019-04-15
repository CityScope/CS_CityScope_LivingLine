#pragma once


struct gridjson {
  float x;
  float y;
  float rot;
  int type;

  ofJson toJson() const {
    ofJson json;
    json["x"] = x;
    json["y"] = y;
    json["rot"] = rot;
    json["type"] = type;
    return json;
}
};
