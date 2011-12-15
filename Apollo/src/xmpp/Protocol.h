// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Protocol_h_INCLUDED)
#define Protocol_h_INCLUDED

#define JABBER_PRESENCE_AVAILABLE "available"
#define JABBER_PRESENCE_UNAVAILABLE "unavailable"
#define JABBER_PRESENCE_ERROR "error"

#define JABBER_NS_AUTH "jabber:iq:auth"
#define JABBER_NS_MUC_USER "http://jabber.org/protocol/muc#user"
#define JABBER_NS_VERSION "jabber:iq:version"
#define JABBER_NS_DELAY "jabber:x:delay"
#define JABBER_NS_DELAY2 "urn:xmpp:delay"

#define JABBER_ERRORCODE_Conflict 409

#define NS_PRESENCE_X_USER_IDENTITY_LEGACY "firebat:user:identity"
#define NS_PRESENCE_X_USER_JID_LEGACY "firebat:user:jid"
#define NS_PRESENCE_X_AVATAR_STATE_LEGACY "firebat:avatar:state"
#define NS_PRESENCE_X_FEATURES_LEGACY "firebat:features"

#define NS_PRESENCE_X_IDENTITY "vp:identity"
#define NS_PRESENCE_X_STATE "vp:state"

/*
#define JABBER_NS_ROSTER "jabber:iq:roster"
#define JABBER_NS_EVENT "jabber:x:event"
#define JABBER_NS_DATA "jabber:x:data"

#define JABBER_NS_LAST "jabber:iq:last"
#define JABBER_NS_TIME "jabber:iq:time"
#define JABBER_NS_REGISTER "jabber:iq:register"
#define JABBER_NS_BROWSE "jabber:iq:browse"

#define JABBER_NS_WHITEBOARD "http://jabber.org/protocol/swb"

#define JABBER_NS_MUC "http://jabber.org/protocol/muc"
#define JABBER_NS_MUC_OWNER "http://jabber.org/protocol/muc#owner"
#define JABBER_NS_MUC_ADMIN "http://jabber.org/protocol/muc#admin"

#define JABBER_ERRORCODE_NoError 0
#define JABBER_ERRORCODE_Redirect 302 
#define JABBER_ERRORCODE_Bad_Request 400 
#define JABBER_ERRORCODE_Unauthorized 401 
#define JABBER_ERRORCODE_Payment_Required 402 
#define JABBER_ERRORCODE_Forbidden 403 
#define JABBER_ERRORCODE_Not_Found 404 
#define JABBER_ERRORCODE_Not_Allowed 405 
#define JABBER_ERRORCODE_Not_Acceptable 406 
#define JABBER_ERRORCODE_Registration_Required 407 
#define JABBER_ERRORCODE_Request_Timeout 408 
#define JABBER_ERRORCODE_Conflict 409
#define JABBER_ERRORCODE_Internal_Server_Error 500 
#define JABBER_ERRORCODE_Not_Implemented 501 
#define JABBER_ERRORCODE_Internal_Timeout 502 // MUC sends this if too much traffic, must check the CData: <error code='502'>Internal_Timeout</error>
#define JABBER_ERRORCODE_Remote_Server_Error 502 
#define JABBER_ERRORCODE_Service_Unavailable 503 
#define JABBER_ERRORCODE_Remote_Server_Timeout 504 

#define FB_NS_QUERY_PROFILE_STORAGE "storage:client:profile"
#define FB_NS_QUERY_AVATAR_STORAGE "storage:client:avatar"
#define FB_NS_QUERY_AVATAR2_STORAGE "storage:client:avatar2"
#define FB_NS_QUERY_LOCATION "http://schema.bluehands.de/location"
#define FB_NS_QUERY_TOPSITE "http://schema.bluehands.de/topsites"
#define FB_NS_QUERY_AVATAR_POSITION "http://schema.bluehands.de/avatar#position"
#define FB_NS_QUERY_COBROW "http://jabber.org/protocol/cobrow"

#define FB_NS_PRESENCE_X_AVATAR_POSITION "firebat:avatar:position"
#define FB_NS_PRESENCE_X_AVATAR_DIGEST "firebat:avatar:digest"
#define FB_NS_PRESENCE_X_AVATAR2_DIGEST "firebat:avatar2:digest"
#define FB_NS_PRESENCE_X_AVATAR_DATA "firebat:avatar:data"
#define FB_NS_PRESENCE_X_AVATAR2_DATA "firebat:avatar2:data"
#define FB_NS_PRESENCE_X_USER_JID "firebat:user:jid"
#define FB_NS_PRESENCE_X_USER_DIGEST "firebat:user:digest"
#define FB_NS_PRESENCE_X_ICON_CUSTOM "firebat:icon:custom"
#define FB_NS_PRESENCE_X_ICON_SYSTEM "firebat:icon:system"
#define FB_NS_PRESENCE_X_ICON_VIDEO "firebat:icon:video"
#define FB_NS_PRESENCE_X_CHAT_ATTRIB "firebat:chat:attrib"

#define FB_NS_MESSAGE_X_AVATAR_POSITION "firebat:avatar:position"
#define FB_NS_MESSAGE_X_AVATAR_GETPOS "firebat:avatar:getpos"
#define FB_NS_MESSAGE_X_CHAT_ACTION "firebat:chat:action"
#define FB_NS_MESSAGE_X_CHAT_KEY "firebat:chat:key"
#define FB_NS_MESSAGE_X_CHAT_STATE "firebat:chat:state"
*/
#endif // !defined(Protocol_h_INCLUDED)
