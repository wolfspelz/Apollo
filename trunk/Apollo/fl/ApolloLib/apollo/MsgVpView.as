package apollo {
	
	public class MsgVpView {
		
		// Location
		public static const Method_Vp_View_EnterLocationRequested:String 		=	'Vp_View_EnterLocationRequested';
		public static const Method_Vp_View_EnterLocationBegin:String 			=	'Vp_View_EnterLocationBegin';
		public static const Method_Vp_View_EnterLocationComplete:String 		=	'Vp_View_EnterLocationComplete';
		
		public static const Method_Vp_View_LeaveLocationRequested:String		=	'Vp_View_LeaveLocationRequested';
		public static const Method_Vp_View_LeaveLocationBegin:String			=	'Vp_View_LeaveLocationBegin';
		public static const Method_Vp_View_LeaveLocationComplete:String			=	'Vp_View_LeaveLocationComplete';
		
		public static const Method_Vp_View_LocationsChanged:String				=	'Vp_View_LocationsChanged';
		public static const Method_Vp_View_LocationDetailsChanged:String		=	'Vp_View_LocationDetailsChanged';
		public static const Method_Vp_View_LocationContextsChanged:String		=	'Vp_View_LocationContextsChanged';

		public static const Method_Vp_View_GetLocations:String					=	'Vp_View_GetLocations';
		public static const Method_Vp_View_GetLocationContexts:String			=	'Vp_View_GetLocationContexts';
		public static const Method_Vp_View_GetLocationDetail:String				=	'Vp_View_GetLocationDetail';
		public static const Method_Vp_View_SubscribeLocationDetail:String		=	'Vp_View_SubscribeLocationDetail';
		public static const Method_Vp_View_UnsubscribeLocationDetail:String		=	'Vp_View_UnsubscribeLocationDetail';
		
		// Chat
		public static const Method_Vp_View_LocationPublicChat:String			=	'Vp_View_LocationPublicChat';
		public static const Method_Vp_View_ReplayLocationPublicChat:String		=	'Vp_View_ReplayLocationPublicChat';
		
		//Context
		public static const Method_Vp_View_ContextLocationAssigned:String		=	'Vp_View_ContextLocationAssigned';
		public static const Method_Vp_View_ContextLocationUnassigned:String		=	'Vp_View_ContextLocationUnassigned';
		public static const Method_Vp_View_ContextDetailsChanged:String			=	'Vp_View_ContextDetailsChanged';
		
		public static const Method_Vp_View_GetContextDetail:String				=	'Vp_View_GetContextDetail';
		public static const Method_Vp_View_SubscribeContextDetail:String		=	'Vp_View_SubscribeContextDetail';
		public static const Method_Vp_View_UnsubscribeContextDetail:String		=	'Vp_View_UnsubscribeContextDetail';
			
		// Participant
		public static const Method_Vp_View_ParticipantsChanged:String			=	'Vp_View_ParticipantsChanged';
		public static const Method_Vp_View_ParticipantDetailsChanged:String		=	'Vp_View_ParticipantDetailsChanged';		
		public static const Method_Vp_View_ParticipantAdded:String				=   'Vp_View_ParticipantAdded';
		public static const Method_Vp_View_ParticipantRemoved:String			=	'Vp_View_ParticipantRemoved';
		
		public static const Method_Vp_View_GetParticipants:String				=	'Vp_View_GetParticipants';
		public static const Method_Vp_View_GetParticipantDetail:String			=   'Vp_View_GetParticipantDetail';
		public static const Method_Vp_View_GetParticipantDetailRef:String		= 	'Vp_View_GetParticipantDetailRef';
		public static const Method_Vp_View_SubscribeParticipantDetail:String	=	'Vp_View_SubscribeParticipantDetail';
		public static const Method_Vp_View_UnsubscribeParticipantDetail:String	 =	'Vp_View_UnsubscribeParticipantDetail';
		
		// Profile
		public static const Method_Vp_View_Profile_Create:String						=	'Vp_View_Profile_Create';
		public static const Method_Vp_View_Profile_Delete:String						=	'Vp_View_Profile_Delete';

		public static const Method_Vp_View_Profile_SetParticipantDetail:String			=	'Vp_View_Profile_SetParticipantDetail';
		public static const Method_Vp_View_Profile_GetParticipantDetails:String			=	'Vp_View_Profile_GetParticipantDetails';
		
		public static const Method_Vp_View_Profile_SubscribeParticipantDetails:String	=	'Vp_View_Profile_SubscribeParticipantDetails';
		public static const Method_Vp_View_Profile_UnsubscribeParticipantDetails:String	=	'Vp_View_Profile_UnsubscribeParticipantDetails';

		// Other stuff
		public static const Method_Vp_SendPublicChat:String								=	'Vp_SendPublicChat';
		
		public static const Method_Vp_NavigateContext:String							=	'Vp_NavigateContext';
		public static const Method_Vp_CloseContext:String								=	'Vp_CloseContext';
		
		public static const Method_Vp_View_IdentityContainerAvailable:String	=	'Vp_View_IdentityContainerAvailable';
		
		
		public function MsgVpView() {
		}

	}
}