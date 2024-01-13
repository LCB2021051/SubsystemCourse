// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"

void PrintString(const FString& str )
{
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan,str);
    }
}

UMultiplayerSessionSubsystem::UMultiplayerSessionSubsystem()
{
    // PrintString("MSS Constructor");

    CreateServerAfterDestroy = false;
    DestroyServerName = "";
    ServerNameToFind = "";
    MySessionName = FName("Co-op Adventure Session Name");
    GameMapPath = "";
}

void UMultiplayerSessionSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
    // PrintString("MSS Initialize");
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

    if(OnlineSubsystem)
    {
        FString str = OnlineSubsystem->GetSubsystemName().ToString();
        PrintString(str);

        SessionInterface = OnlineSubsystem->GetSessionInterface();

        if(SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, 
            &UMultiplayerSessionSubsystem::OnCreateSessionComplete);

            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this,
            &UMultiplayerSessionSubsystem::OnDestroySessionComplete); 

            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,
            &UMultiplayerSessionSubsystem::OnFindSessionsComplete);

            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,
            &UMultiplayerSessionSubsystem::OnJoinSessionComplete);
        }
    }
}
void UMultiplayerSessionSubsystem::Deinitialize()
{
    // UE_LOG(LogTemp, Warning, TEXT("Deinitialize"));
}

void UMultiplayerSessionSubsystem::CreateServer(FString ServerName)
{
    PrintString("Create Server");

    if(ServerName.IsEmpty())
    {
        PrintString("Server Name Cannot be empty!");
        ServerCreateDel.Broadcast(false);
        return;
    }

    FNamedOnlineSession *ExistingSession = SessionInterface->GetNamedSession(MySessionName); //checking if a session with this name already exixts.

    if(ExistingSession)
    {
        FString Msg = FString::Printf(TEXT("Session with name %s already exists, destroying it."),
        *MySessionName.ToString());
        PrintString(Msg);
        CreateServerAfterDestroy = true;
        DestroyServerName = ServerName;
        SessionInterface->DestroySession(MySessionName);
        return;
    }

    FOnlineSessionSettings SessionSettings;
    
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bIsDedicated = false;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.NumPublicConnections = 2;
    SessionSettings.bUseLobbiesIfAvailable = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinViaPresence = true;

    bool IsLAN = false;
    if(IOnlineSubsystem::Get()->GetSubsystemName().ToString() == "NULL")
    {
        IsLAN = true;
    } 
    SessionSettings.bIsLANMatch = IsLAN;

    SessionSettings.Set(FName("SERVER_NAME"),ServerName,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);    

    SessionInterface->CreateSession(0,MySessionName,SessionSettings);
}

void UMultiplayerSessionSubsystem::FindServer(FString ServerName)
{
    if(ServerName.IsEmpty())
    {
        PrintString("Server Name Cannot be Empty");
        ServerJoinDel.Broadcast(false);
        return;
    }
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    bool IsLAN = false;
    if(IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
    {
        IsLAN = true;
    }
    SessionSearch->bIsLanQuery = IsLAN;
    SessionSearch->MaxSearchResults = 9999;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE,true,EOnlineComparisonOp::Equals);

    ServerNameToFind = ServerName;

    SessionInterface->FindSessions(0,SessionSearch.ToSharedRef());
}

void UMultiplayerSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool WasSussesful)
{
    PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"),WasSussesful));

    ServerCreateDel.Broadcast(WasSussesful);

    if(WasSussesful)
    {
        FString Path = "/Game/ThirdPerson/Maps/ThirdPersonMap?Listen" ;
        if(!GameMapPath.IsEmpty())
        {
            Path = FString::Printf(TEXT("%s?Listen"),*GameMapPath);
        }
        GetWorld()->ServerTravel(Path);
    }
}

void UMultiplayerSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool WasSussesful)
{
    FString Msg = FString::Printf(TEXT("OnDestroySessionComplete, SessionName: %s, Succesful: %d"),
    *SessionName.ToString(),WasSussesful);
    PrintString(Msg);
    if(CreateServerAfterDestroy)
    {
        CreateServerAfterDestroy = false;
        CreateServer(DestroyServerName);
    }
}

void UMultiplayerSessionSubsystem::OnFindSessionsComplete(bool WasSussesful)
{
    if(!WasSussesful) return;
    if(ServerNameToFind.IsEmpty()) return;

    TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;

    if(Results.Num() > 0)
    {
        FString Msg = FString::Printf(TEXT("%d Result Found"),Results.Num());
        PrintString(Msg);

        FOnlineSessionSearchResult *CorrectResult = 0;

        for(FOnlineSessionSearchResult Result:Results)
        {
            if(Result.IsValid())
            {
                FString ServerName = "No-Name";
                Result.Session.SessionSettings.Get(FName("SERVER_NAME"),ServerName);

                if(ServerName.Equals(ServerNameToFind))
                {
                    CorrectResult = &Result; //server
                    FString Msg2 = FString::Printf(TEXT("Server found With Name: %s"),*ServerName);
                    PrintString(Msg2);
                    break;
                }
            }
        }

        if(CorrectResult)
        {
            SessionInterface->JoinSession(0,MySessionName,*CorrectResult);
        }
        else
        {
            PrintString(FString::Printf(TEXT("Couldn't Find Server : %s"),*ServerNameToFind));
            ServerJoinDel.Broadcast(false);
            ServerNameToFind = "";
        }
    }
    else
    {
        PrintString("No Result Found");
        ServerJoinDel.Broadcast(false);
    }
}

void UMultiplayerSessionSubsystem::OnJoinSessionComplete(FName SessionName,EOnJoinSessionCompleteResult::Type Result)
{
    ServerJoinDel.Broadcast(Result == EOnJoinSessionCompleteResult::Success);
    if(Result == EOnJoinSessionCompleteResult::Success)
    {
        PrintString(FString::Printf(TEXT("Session Joined: %s"),*SessionName.ToString()));

        FString Address = "";
        bool Success = SessionInterface->GetResolvedConnectString(MySessionName,Address);

        if(Success)
        {
            PrintString(FString::Printf(TEXT("Address: %s"),*Address));
            APlayerController *PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if(PlayerController)
            {
                PlayerController->ClientTravel(Address,ETravelType::TRAVEL_Absolute);
            }
        }
        else
        {
            PrintString("GetResolvedConnectString returned false");
        }
    }
    else
    {
        PrintString("OnJoinSessionComplete: FAILED");
    }
}
