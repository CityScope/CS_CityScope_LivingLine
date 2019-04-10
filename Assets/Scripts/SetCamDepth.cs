using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SetCamDepth : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void SetDepth(bool toggleValue)
    {
        if (toggleValue)
        {
            GetComponent<Camera>().depth = 0;
        }
        else
        {
            GetComponent<Camera>().depth = -1;
        }
    }
}
