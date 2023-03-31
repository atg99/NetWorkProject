// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

//생성자
UServerGameInstance::UServerGameInstance()
{
	
}

//비긴플레이 생성자에 가까운 초기함수
void UServerGameInstance::Init()
{
	//게임 인스턴스가 초기화 되어야만 월드가 생성되도 게임모드가 생성되고 다음이 생성된다
	Super::Init();

	//세션 열기 (스트리밍을 연다) 세션 생성

	//온라인 세션 기능이 구현되어 있는 IOnlineSubsystem 클래스를 가져온다
	IOnlineSubsystem* subsys = IOnlineSubsystem::Get();
	if(subsys)
	{
		
		sessionInterface = subsys->GetSessionInterface();
		if(sessionInterface != nullptr)
		{
			sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UServerGameInstance::OnCreateSessionComplete);
			sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UServerGameInstance::OnFindSessionComplete); // 세선 검색 완료(비동기)
			//네이티브 c++ UFunction 불필요
			sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UServerGameInstance::OnJoinSessionComplete);	//세션 참여 완료
		}
	}

	//연결 플랫폼의 이름을 출력한다
	//subsys->GetInstanceName()
	FString platformName = subsys->GetSubsystemName().ToString();
	UE_LOG(LogTemp, Warning, TEXT("PlatformNmae : %s"), *platformName);
}

void UServerGameInstance::CreateMySession(FString roomName, int32 playerCount)
{
	if(sessionInterface != nullptr)
	{
	
		FOnlineSessionSettings SessionSettings; 	//세션 생성 정보를 만든다
		SessionSettings.bAllowInvites = true;  	// 초대 허용
		SessionSettings.bAllowJoinInProgress = true;	//세션실행중에 조인 할 수 있는지 여부
		
		SessionSettings.bAllowJoinViaPresence = true;	// (서버트레블로 가고자하는 레벨이 있는지 여부) 없는 레벨생기면 다른 업데이트 안할 클라이언트가 들어오려하면 오류가나니까 못들어 오게 (내가 연 레벨을 가지고 있는 사람만 들어올 수 있게)
		SessionSettings.bShouldAdvertise = true;	//상대방이 검색할 수 있게하는 여부, 위2개는 같은 값이 좋다 //공개 세션인지 아닌지 (비번방 공개방) 외부검색
		
		SessionSettings.bIsDedicated = false;
		SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";	// 온라인 서브시스템의 이름이 Null이면 Lan 매칭을 하고, Steam이면 스팀 서바로 매칭한다.
		SessionSettings.NumPublicConnections = playerCount;	//최대 몇명 까지 연결을 허용 할 수 있는지 여부
		//SessionSettings.Set(SEARCH_KEYWORDS);

		//SessionSettings.bUseLobbiesIfAvailable = true;	//EOS 음성 커뮤니케이션
		//SessionSettings.bUseLobbiesVoiceChatIfAvailable = true;
		
		SessionSettings.Set(FName("KEY_RoomName"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);	//서비스와 핑을 제공한다		//상대방이 나를 검색할 때의 필터조건
		
		
		//세션을 생성한다
		sessionInterface->CreateSession(0, sessionID, SessionSettings);

		UE_LOG(LogTemp, Warning, TEXT("Create Session try"));
	}
}

void UServerGameInstance::OnCreateSessionComplete(FName sessionName, bool bIsSuccess)
{
	FString result = bIsSuccess ? TEXT("Create Session Success!") : TEXT("Create Session failed");

	UE_LOG(LogTemp, Warning, TEXT("%s : %s"), *result, *sessionName.ToString());

	//세션 생성에 성공 했다면, 전투 맵 쪽으로 인원을 이동 시킨다
	//오픈 레벨은 클라이언트가 직접 이동
	if(bIsSuccess)
	{
		//경로명은 컨텐츠 명으로? Game/-> 서버에서 레벨을 이동 할 때 맵이름 하나만
		GetWorld()->ServerTravel("/Game/Maps/MainMap?Listen");
	}
}

void UServerGameInstance::FindMySession()
{

	sessionSearch = MakeShareable(new FOnlineSessionSearch());		//쉐어드 포인터를 만든다
	//TSharedRef<FOnlineSessionSearch> sessionSearch = MakeShareable(new FOnlineSessionSearch());	//이건 왜 안됨?

	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	sessionSearch-> MaxSearchResults = 30; //한번에 찾을 세션의 수
	//특정 키로 검색 조건을 필터링 하고 싶을 때 추가하는 쿼리
	//sessionSearch->QuerySettings.Set(FName("KEY_RoomName"), roomName, EOnlineComparisonOp::GreaterThan); //EOnlineComparisonOp::GreaterThan 검색조건 포함, 동일, 제외

	//Presence로 생성된 세션을 필터링 하고 싶을 때

	// 	/** Search only for dedicated servers (value is true/false) */
	// #define SEARCH_DEDICATED_ONLY FName(TEXT("DEDICATEDONLY"))
	// /** Search for empty servers only (value is true/false) */
	// #define SEARCH_EMPTY_SERVERS_ONLY FName(TEXT("EMPTYONLY"))
	// /** Search for non empty servers only (value is true/false) */
	// #define SEARCH_NONEMPTY_SERVERS_ONLY FName(TEXT("NONEMPTYONLY"))
	// /** Search for secure servers only (value is true/false) */
	// #define SEARCH_SECURE_SERVERS_ONLY FName(TEXT("SECUREONLY"))
	// /** Search for presence sessions only (value is true/false) */
	// #define SEARCH_PRESENCE FName(TEXT("PRESENCESEARCH"))
	// /** Search for a match with min player availability (value is int) */
	// #define SEARCH_MINSLOTSAVAILABLE FName(TEXT("MINSLOTSAVAILABLE"))
	// /** Exclude all matches where any unique ids in a given array are present (value is string of the form "uniqueid1;uniqueid2;uniqueid3") */
	// #define SEARCH_EXCLUDE_UNIQUEIDS FName(TEXT("EXCLUDEUNIQUEIDS"))
	// /** User ID to search for session of */
	// #define SEARCH_USER FName(TEXT("SEARCHUSER"))
	// /** Keywords to match in session search */
	// #define SEARCH_KEYWORDS FName(TEXT("SEARCHKEYWORDS"))
	// /** The matchmaking queue name to matchmake in, e.g. "TeamDeathmatch" (value is string) */
	// #define SEARCH_MATCHMAKING_QUEUE FName(TEXT("MATCHMAKINGQUEUE"))
	// /** If set, use the named Xbox Live hopper to find a session via matchmaking (value is a string) */
	// #define SEARCH_XBOX_LIVE_HOPPER_NAME FName(TEXT("LIVEHOPPERNAME"))
	// /** Which session template from the service configuration to use */
	// #define SEARCH_XBOX_LIVE_SESSION_TEMPLATE_NAME FName(TEXT("LIVESESSIONTEMPLATE"))
	// /** Selection method used to determine which match to join when multiple are returned (valid only on Switch) */
	// #define SEARCH_SWITCH_SELECTION_METHOD FName(TEXT("SWITCHSELECTIONMETHOD"))
	// /** Whether to use lobbies vs sessions */
	// #define SEARCH_LOBBIES FName(TEXT("LOBBYSEARCH"))
	//
	// // User attributes for searching (FSessionMatchmakingUser::Attributes)
	// /** Team a user is searching for */
	// #define SEARCH_USER_ATTRIBUTE_TEAM TEXT("TEAM")
	
	sessionSearch -> QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	
	
	// 앞에서 만든 쿼리를 이용해서 세션을 찾는다
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

// 찾은 세션 리스트 중에서 특정 세션에 들어가고자 할 때 사용할 함수
void UServerGameInstance::JoinMySession(int32 sessionIDX)
{
	FOnlineSessionSearchResult selectedSession = sessionSearch -> SearchResults[sessionIDX];

	sessionInterface->JoinSession(0, sessionID, selectedSession);
}

// 세션 검색이 끝났을 때 호출될 함수 (비동기)
void UServerGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	//검색 성공
	if(bWasSuccessful)
	{
		TArray<FOnlineSessionSearchResult> searchResults = sessionSearch->SearchResults; //세션 검색 결과 배열을 복사한다
		UE_LOG(LogTemp, Warning, TEXT("Find Session count : %d"), searchResults.Num()); // 검색 결과 수

		//검색 결과를 순회한다
		for(int32 i = 0; i < searchResults.Num(); i++)
		{
			FSessionInfo searchedSessionInfo;
			
			FString roomName;
			searchResults[i].Session.SessionSettings.Get(FName("KEY_RoomName"), roomName);
			searchedSessionInfo.roomName = roomName;
			
			searchedSessionInfo.maxPlayers = searchResults[i].Session.SessionSettings.NumPublicConnections;
			searchedSessionInfo.currentPlayers = searchedSessionInfo.maxPlayers - searchResults[i].Session.NumOpenPublicConnections;	//최대 플레이어 - 현재 비어있는 커넥션 수 (비어있는 자리의 수) == 현재 플레이어

			searchedSessionInfo.ping = searchResults[i].PingInMs;

			searchedSessionInfo.idx = i;
			
			searchResultDele.Broadcast(searchedSessionInfo);
		}
	}
	else if(!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Find sessuions failed"));
	}

	//검색이 끝나면 버튼을 다시 활성화 한다
	searchFinshed.Broadcast();
}

//다른 세션에 함류 처리가 끝났을 때 호출되는 이벤트 함수
void UServerGameInstance::OnJoinSessionComplete(FName sessionName, enum EOnJoinSessionCompleteResult::Type joinResult)
{
	//만일 join에 성공 했다면 해당 IP Address로 레벨 이동을 한다
	if(joinResult == EOnJoinSessionCompleteResult::Success)
	{
		//세션이름을 넣으면 세션이름에 해당하는 IPAddress와 주소를 넘겨주는 함수
		//세션 이름으로 IP주소를 탐색한다
		FString joinAddress;
		sessionInterface->GetResolvedConnectString(sessionName, joinAddress);

		UE_LOG(LogTemp, Warning, TEXT("join Address : %s"), *joinAddress);
		
		if(APlayerController* pc = GetWorld()->GetFirstPlayerController())
		{
			//절대경로 내가 가지고 있는 레벨로 이동이 아닌 서버ip가 가지고 있는 맵의 주소로 가기 때문에 절대경로
			pc->ClientTravel(joinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
}



