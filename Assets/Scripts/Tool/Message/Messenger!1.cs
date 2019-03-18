using System;
using System.Collections.Generic;

namespace DMessager
{
	public static class Messenger<M>
	{
	    private static Dictionary<M, Delegate> eventTable;
	
	    static Messenger()
	    {
	        Messenger<M>.eventTable = MessengerInternal<M>.eventTable;
	    }
	
	    public static void addListener(M messageType, OnMessageDelegate<M> handler)
	    {
	        MessengerInternal<M>.onAddListener(messageType, handler);
	        Messenger<M>.eventTable[messageType] = (OnMessageDelegate<M>) Delegate.Combine((OnMessageDelegate<M>) Messenger<M>.eventTable[messageType], handler);
	    }
	
	    public static void postMessage(M messageType)
	    {
	        Delegate delegate2;
	        if (Messenger<M>.eventTable.TryGetValue(messageType, out delegate2))
	        {
	            OnMessageDelegate<M> delegate3 = delegate2 as OnMessageDelegate<M>;
	            if (delegate3 != null)
	            {
	                delegate3();
	            }
	        }
	    }
	
	    public static void removeListener(M messageType, OnMessageDelegate<M> handler)
	    {
	        if (Messenger<M>.eventTable.ContainsKey(messageType))
	        {
	            Messenger<M>.eventTable[messageType] = (OnMessageDelegate<M>) Delegate.Remove((OnMessageDelegate<M>) Messenger<M>.eventTable[messageType], handler);
	            MessengerInternal<M>.onRemoveListener(messageType);
	        }
	    }
	}
}

