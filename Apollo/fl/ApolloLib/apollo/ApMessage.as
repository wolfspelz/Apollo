package apollo
{	
	import apollo.SrpcMessage;
	
	public class ApMessage extends SrpcMessage
	{
		public static const Param_MessageType:String = "ApType";
		
		public function ApMessage(sType:String = ""):void
    {
			if (sType != "") {
				this.setString(ApMessage.Param_MessageType, sType);
			}
    }

	}
}
