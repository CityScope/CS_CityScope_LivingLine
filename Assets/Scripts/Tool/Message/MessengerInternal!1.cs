using System;
using System.Collections.Generic;

namespace DMessager
{
	internal static class MessengerInternal<M>
	{
	    public static Dictionary<M, Delegate> eventTable;
	
	    static MessengerInternal()
	    {
	        MessengerInternal<M>.eventTable = new Dictionary<M, Delegate>();
	    }
	
	    public static void onAddListener(M messageType, Delegate listenerBeingAdded)
	    {
	        if (!MessengerInternal<M>.eventTable.ContainsKey(messageType))
	        {
	            MessengerInternal<M>.eventTable.Add(messageType, null);
	        }
	    }
	
	    public static void onRemoveListener(M messageType)
	    {
	        if (MessengerInternal<M>.eventTable[messageType] == null)
	        {
	            MessengerInternal<M>.eventTable.Remove(messageType);
	        }
	    }
	}
}

