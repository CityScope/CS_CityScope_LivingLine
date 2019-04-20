using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DisplayCtrl : MonoBehaviour
{

    [Range(1,5)]
    public int maxDisplays = 3;
    
    void Start()
    {
        Debug.Log("displays connected: " + Display.displays.Length);

        // Display.displays[0] is the primary, default display and is always ON.

        // Check if additional displays are available and activate each.
        if (Display.displays.Length > 1 && maxDisplays > 1)
            Display.displays[1].Activate();
        if (Display.displays.Length > 2 && maxDisplays > 2)
            Display.displays[2].Activate();
        if (Display.displays.Length > 3 && maxDisplays > 3)
            Display.displays[3].Activate();
        if (Display.displays.Length > 4 && maxDisplays > 4)
            Display.displays[4].Activate();

    }

}
