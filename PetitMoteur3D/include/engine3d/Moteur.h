#pragma once
#include "Singleton.h"
#include "dispositif.h"


#include <string>
#include <codecvt>
#include <locale>
#include <chrono>
#include <vector>

#include "CollisionHandler.h"
#include "GestionnaireDeTextures.h"
#include "MeshInstanceManager.h"
#include "ShaderInstanceManager.h"
#include "InputController.h"
#include "cameraController.h"
#include "UIController.h"
#include "Vehicule.h"
#include "Objet3D.h"
#include "Bloc.h"
#include "BlocEffet1.h"
#include "MeshInstance.h"
#include "GameObject.h"
#include "GeometryInitData.h"
#include "Terrain.h"
#include "DIManipulateur.h"
#include "TerrainHandler.h"
#include "ChargeurOBJ.h"
#include "PxPhysicsAPI.h"
#include "Light.h"
#include "Converter.h"
#include "ObjectPhysicalInstance.h"
#include "DeadZone.h"
#include "TimeController.h"
#include "GameStateManager.h"
#include "TriggerZoneCave.h"
#include "Monstre.h"
#include "GameStateManager.h"
#include "SoundController.h"
#include "TriggerZoneTp.h"
#include "RenderTexture.h"

#include "AfficheurSprite.h"
#include "AfficheurTexte.h"

using namespace physx;
namespace PM3D
{
	//couleur a appliquer lorsque la surface de rendu est effacee
	constexpr const float CLEARCOLOR[4] = { 0.0f, 0.7f, 1.0f, 1.0f };
	constexpr const float CLEARBLACK[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	constexpr const float CLEARRED[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	
	constexpr const int DIMRENDERTEXTURE = 16384;

	//caracteristiques du frustum, utiles partout
	constexpr const float NEARPLANE = 2.0;
	constexpr const float FARPLANE = 200000.0;
	constexpr const float FOV = XM_PI / 4; 	// 45 degrés

	const int IMAGESPARSECONDE = 60;
	const double EcartTemps = 1.0 / static_cast<double>(IMAGESPARSECONDE);

	//
	//   TEMPLATE : CMoteur
	//
	//   BUT : Template servant à construire un objet Moteur qui implantera les
	//         aspects "génériques" du moteur de jeu
	//
	//   COMMENTAIRES :
	//
	//        Comme plusieurs de nos objets représenteront des éléments uniques 
	//        du système (ex: le moteur lui-même, le lien vers 
	//        le dispositif Direct3D), l'utilisation d'un singleton 
	//        nous simplifiera plusieurs aspects.
	//
	template <class T, class TClasseDispositif> class CMoteur :public CSingleton<T>
	{
		PxDefaultAllocator		gAllocator;
		PxDefaultErrorCallback	gErrorCallback;

		PxFoundation* gFoundation = NULL;
		PxPhysics* gPhysics = NULL;

		PxCooking* gCooking = NULL;

		PxDefaultCpuDispatcher* gDispatcher = NULL;
		PxScene* gScene = NULL;

		PxMaterial* gMaterial = NULL;

		PxPvd* gPvd = NULL;

		PxReal stackZ = 10.0f;
	public:

		void ResetGame()
		{
			cameraController.setActiveCamera(0);
			vehicule->teleport(PxVec3(0.f, 50.f, 0.f), 1.5f);
			cameraController.updateFOVSpeeds();
			monstres->ResetMonstres();
			gScene->setGravity(PxVec3(0.0f, -30.0f, 0.0f));
			for (Light& light : listeLumieres) {
				if (light.isSun)
				{
					light.ambiant = XMVectorSet(.40f, .40f, .50f, 1.0f);
					light.diffuse = XMVectorSet(0.8f, 0.75f, 0.70f, 1.0f);
					light.specular = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
					break;
				}
			}
			monstres->setVitesse(vehicule->getVitMax() * difficulte);
				
			TimeController::GetInstance().restart();
		}

		virtual void Run()
		{
			bool bBoucle = true;

			while (bBoucle)
			{
				// Propre à la plateforme - (Conditions d'arrêt, interface, messages)
				bBoucle = RunSpecific();
				GameStateManager::GetInstance().executeCalls();
				// appeler la fonction d'animation
				if (bBoucle)
				{
					bBoucle = Animation();
				}
			}
		}

		virtual int Initialisations()
		{
			tempsEntreLumiere = 0.0f;
			tempsEntreTP = 0.0f;
			// Propre à la plateforme
			InitialisationsSpecific();

			// * Initialisation du dispositif de rendu
			pDispositif = CreationDispositifSpecific(CDS_PLEIN_ECRAN);
			initUIObjects();
			// * Initialisation de la scène
			InitScene();
			renderTexture.init(pDispositif->GetD3DDevice(), DIMRENDERTEXTURE, DIMRENDERTEXTURE);
			//InitRenderTextures(listeLumieres.size());
			// * Initialisation des paramètres de l'animation et 
			//   préparation de la première image
			InitAnimation();


			return 0;
		}

		virtual bool Animation()
		{
			// méthode pour lire l'heure et calculer le 
			// temps écoulé
			const int64_t TempsCompteurCourant = GetTimeSpecific();
			const double TempsEcoule = GetTimeIntervalsInSec(TempsCompteurPrecedent, TempsCompteurCourant);

			

			// Est-il temps de rendre l'image?
			if (TempsEcoule > EcartTemps)
			{
				tempsEntreLumiere -= TempsEcoule;
				tempsEntreTP -= TempsEcoule;
				// Affichage optimisé
				pDispositif->Present();
				// On prépare la prochaine image
				AnimeScene(static_cast<float>(TempsEcoule));
				// On rend l'image sur la surface de travail
				// (tampon d'arrière plan)
				RenderSceneUI(GameStateManager::GetInstance().getInGame());

				// Calcul du temps du prochain affichage
				TempsCompteurPrecedent = TempsCompteurCourant;
			}

			return true;
		}

		void Teleport(bool isInMoon)
		{
			if (isInMoon)
			{
				// TP SUR LA TERRE
				vehicule->teleport(PxVec3(0.f,50.f,0.f), 1.5f);
				monstres->ResetMonstres();
				gScene->setGravity(PxVec3(0.0f, -30.0f, 0.0f));
			}
			else
			{
				// TP SUR LA LUNE
				vehicule->teleport(PxVec3(-138, 2220, 16),1.5f);
				gScene->setGravity(PxVec3(0.0f, -10.0f, 0.0f));
			}
		}

#pragma region getters  setters
		TClasseDispositif* GetDispositif() { return pDispositif; }
		const XMMATRIX& GetMatView() const { return m_MatView; }
		const XMMATRIX& GetMatProj() const { return m_MatProj; }
		const XMMATRIX& GetMatViewProj() const { return m_MatViewProj; }
		void SetMatView(const XMMATRIX& matView) const { m_MatView = matView; }
		void SetMatProj(const XMMATRIX& matProj) const { m_MatProj = matProj; }
		void SetMatViewProj(const XMMATRIX& matViewProj) const { matViewProj = matViewProj; }
		CDIManipulateur& GetGestionnaireDeSaisie() { return GestionnaireDeSaisie; }
		CCamera* GetCamera() { return cameraController.getActiveCamera(); }
		std::vector<std::unique_ptr<CObjet3D>>& getScene() { return ListeScene;  }
		std::vector <Light>& getLights() { return listeLumieres; }
		CGestionnaireDeTextures& GetTextureManager() { return TexturesManager; }
		CMeshInstanceManager& GetMeshInstanceManager() { return MeshInstanceManager; }
		ShaderInstanceManager& GetShaderInstanceManager() { return ShaderInstanceManager; }
		RenderTexture& GetRenderTexture() { return renderTexture; }
		PxFoundation* getPxFoundation() {
			return gFoundation;
		}

		PxPhysics* getPxPhysics() {
			return gPhysics;
		}

		PxCooking* getPxCooking() {
			return gCooking;
		}

		PxDefaultCpuDispatcher* getPxDispatcher() {
			return gDispatcher;
		}

		PxScene* getPxScene() {
			return gScene;
		}

		PxMaterial* getPxMaterial() {
			return gMaterial;
		}

		PxPvd* getPxPvd() {
			return gPvd;
		}

		PxReal getPxStackZ() {
			return stackZ;
		}

		void setTempsEntreLumiere(double _tempsEntreLumiere)
		{
			tempsEntreLumiere = _tempsEntreLumiere;
		}

		double getTempsEntreLumiere()
		{
			return tempsEntreLumiere;
		}

		void setTempsEntreTP(double _tempsEntreTP)
		{
			tempsEntreTP= _tempsEntreTP;
		}

		double getTempsEntreTP()
		{
			return tempsEntreTP;
		}
#pragma endregion

	protected:

		virtual ~CMoteur()
		{
			Cleanup();
		}

		// Spécifiques - Doivent être implantés
		virtual bool RunSpecific() = 0;
		virtual int InitialisationsSpecific() = 0;

		virtual int64_t GetTimeSpecific() const = 0;
		virtual double GetTimeIntervalsInSec(int64_t start, int64_t stop) const = 0;

		virtual TClasseDispositif* CreationDispositifSpecific(const CDS_MODE cdsMode) = 0;
		virtual void BeginRenderSceneSpecific() = 0;
		virtual void EndRenderSceneSpecific() = 0;

		// Autres fonctions
		virtual int InitAnimation()
		{
			TempsSuivant = GetTimeSpecific();
			TempsCompteurPrecedent = TempsSuivant;

			// première Image
			RenderScene();

			return true;
		}

		void stepPhysics()
		{
			if (GameStateManager::GetInstance().getInGame()) {
				gScene->simulate(static_cast<physx::PxReal>(EcartTemps));
				gScene->fetchResults(true);
				PxU32 numberOfActiveActor = 0;
				PxActor** activeActors = gScene->getActiveActors(numberOfActiveActor);
				for (PxU32 i = 0; i < numberOfActiveActor; ++i) {
					static_cast<ObjectPhysicalInstance*>(activeActors[i]->userData)->stepPhysics();
				}
			}		
		}

		static wstring convertStrToWstr(string& orig)
		{
			wstring ws;
			wchar_t* buf = new wchar_t[orig.size() + 1];
			size_t num_chars = mbstowcs(buf, orig.c_str(), orig.size() + 1);
			ws = buf;
			return ws;
			delete[] buf;
		}

		void initUIObjects() {
			CAfficheurTexte::Init();
			const Gdiplus::FontFamily oFamily(L"Agency FB", nullptr);
			pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 45.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

			std::unique_ptr<CAfficheurSprite> pAfficheurMainMenu = std::make_unique<CAfficheurSprite>(pDispositif);
			std::unique_ptr<CAfficheurSprite> pAfficheurHud = std::make_unique<CAfficheurSprite>(pDispositif);
			std::unique_ptr<CAfficheurSprite> pAfficheurMenu = std::make_unique<CAfficheurSprite>(pDispositif);
			//HUD
			pTexteChrono = std::make_unique<CAfficheurTexte>(pDispositif, 140, 50, pPolice.get());
			pTexteSpeed = std::make_unique<CAfficheurTexte>(pDispositif, 170, 50, pPolice.get());
			pTexteMonstre = std::make_unique<CAfficheurTexte>(pDispositif, 170, 50, pPolice.get());
			pTexteTours = std::make_unique<CAfficheurTexte>(pDispositif, 170, 50, pPolice.get());
			pAfficheurHud->AjouterSpriteTexte(pTexteChrono->GetTextureView(), 1780, 50);
			pAfficheurHud->AjouterSpriteTexte(pTexteSpeed->GetTextureView(), 875, 1050);
			pAfficheurHud->AjouterSpriteTexte(pTexteMonstre->GetTextureView(), 0, 1050);
			pAfficheurHud->AjouterSpriteTexte(pTexteTours->GetTextureView(), 1780, 150);
			ListeSceneUI.push_back(std::move(pAfficheurHud));
			//menu
			pTexteReprendre = std::make_unique<CAfficheurTexte>(pDispositif, 240, 50, pPolice.get());
			pTexteReset = std::make_unique<CAfficheurTexte>(pDispositif, 200, 50, pPolice.get());
			pTexteOptions = std::make_unique<CAfficheurTexte>(pDispositif, 200, 50, pPolice.get());
			pTexteQuitter = std::make_unique<CAfficheurTexte>(pDispositif, 200, 50, pPolice.get());
			pAfficheurMenu->AjouterSpriteTexte(pTexteReprendre->GetTextureView(), 855, 300);
			pAfficheurMenu->AjouterSpriteTexte(pTexteReset->GetTextureView(), 895, 450);
			pAfficheurMenu->AjouterSpriteTexte(pTexteOptions->GetTextureView(), 895, 600);
			pAfficheurMenu->AjouterSpriteTexte(pTexteQuitter->GetTextureView(), 895, 750);
			ListeSceneUI.push_back(std::move(pAfficheurMenu));
			//main menu
			pAfficheurMainMenu->AjouterSprite(".\\modeles\\menu\\main_menu_play.dds", 850, 450);
			pAfficheurMainMenu->AjouterSprite(".\\modeles\\menu\\main_menu_options.dds", 850, 600);
			pAfficheurMainMenu->AjouterSprite(".\\modeles\\menu\\main_menu_quitter.dds", 850, 750);
			pAfficheurMainMenu->AjouterSprite(".\\modeles\\menu\\testbackground.dds", 0, 1080, 1920, 1080);
			ListeSceneUI.push_back(std::move(pAfficheurMainMenu));

		}

		int InitUI() {
			std::unique_ptr<CAfficheurSprite> pAfficheurOptions = std::make_unique<CAfficheurSprite>(pDispositif);
			if (GameStateManager::GetInstance().getInOptions()) {
				if(ListeSceneUI.size() > 3) ListeSceneUI.erase(ListeSceneUI.begin() + 3);
				if (activeOption == Option::None) {
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_audio.dds", 850, 200);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_video.dds", 850, 400);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_difficulte.dds", 850, 600);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_touches.dds", 850, 800);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\retour.dds", 850, 1000);
					if (GameStateManager::GetInstance().getInMainMenu()) {
						pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\testbackground.dds", 0, 1080, 1920, 1080);
					}
				}
				else if (activeOption == Option::Audio) {
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_son_25.dds", 850, 200);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_son_50.dds", 850, 400);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_son_75.dds", 850, 600);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_son_100.dds", 850, 800);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\retour.dds", 850, 1000);
				}
				else if (activeOption == Option::Video) {
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_video_43.dds", 850, 200);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_video_54.dds", 850, 400);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_video_169.dds", 850, 600);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_video_1610.dds", 850, 800);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\retour.dds", 850, 1000);
				}
				else if (activeOption == Option::Difficulte) {
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_difficulte_facile.dds", 850, 200);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_difficulte_moyen.dds", 850, 400);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_difficulte_difficile.dds", 850, 600);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_difficulte_fury.dds", 850, 800);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\retour.dds", 850, 1000);
				}
				else if (activeOption == Option::Touche) {
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_son_25.dds", 850, 200);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_son_50.dds", 850, 400);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_son_75.dds", 850, 600);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\options_son_100.dds", 850, 800);
					pAfficheurOptions->AjouterSprite(".\\modeles\\menu\\retour.dds", 850, 1000);
				}
				ListeSceneUI.push_back(std::move(pAfficheurOptions));
				return 3;
			}
			else if (GameStateManager::GetInstance().getInMainMenu()) {
				return 2;
			}
			else {
				if (GameStateManager::GetInstance().getInGame()) {
					return 0;
				}
				else {
					return 1;
				}
			}
			
		}

		

		virtual void RenderSceneUI(bool mode) {
			
			if (!GameStateManager::GetInstance().getInMainMenu()) {
				if (mode) {
					auto timeElapsed = TimeController::GetInstance().getTimeElapsed();
					auto minutes = chrono::duration_cast<chrono::minutes>(timeElapsed);
					auto seconds = chrono::duration_cast<chrono::seconds>(timeElapsed);
					auto mili = chrono::duration_cast<chrono::milliseconds>(timeElapsed);
					string formattedChrono;
					if (mili / 1ms > 100) {
						formattedChrono = "" + to_string(minutes / 1min) + ":" + to_string((seconds / 1s) % 60) + "." + to_string(mili / 1ms).substr(2, 2);
					}
					else formattedChrono = "" + to_string(minutes / 1min) + ":" + to_string((seconds / 1s) % 60) + "." + to_string(mili / 1ms);

					wstring wstr_chrono = convertStrToWstr(formattedChrono);
					wstring wstr_speed;
					if (vehicule->getSpeed() * 4 > (vehicule->getVitMax() * 4) - 6.0f) wstr_speed = to_wstring((int)(vehicule->getVitMax() * 4)) + L"km/h";
					else wstr_speed = to_wstring((int)(vehicule->getSpeed() * 4)) + L"km/h";


					pTexteMonstre->Ecrire(to_wstring((int) monstres->getDistance(vehicule->getPosition())) + L"m") ;
					pTexteTours->Ecrire(L"Tour : " + to_wstring(GameStateManager::GetInstance().getTurnCount()));
					UIController::GetInstance().RenderHUD(pTexteChrono, pTexteSpeed, wstr_chrono, wstr_speed);

				}
				else {
					//pause the timer

					UIController::GetInstance().RenderMenu(pTexteReprendre, pTexteReset, pTexteOptions, pTexteQuitter, selection);

				}
			}
			RenderScene();
		}

		// Fonctions de rendu et de présentation de la scène
		virtual bool RenderScene()
		{
			ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();
			ID3D11DepthStencilView* pDepthStencilView = pDispositif->GetDepthStencilView();

			
			//Rendu vers la texture de rendu a faire ici
			for (auto& light : listeLumieres) {
				if (light.lightType == DIRECTIONAL) {
					renderTexture.beginRender(pDispositif, pImmediateContext, pDepthStencilView);
					for (auto& object3D : ListeScene)
					{
						object3D->BasicRender(light.getViewProjMatrix());
						
					
					}
					renderTexture.endRender(pDispositif, pImmediateContext);
					break;
				}
			}
			

			BeginRenderSceneSpecific();
			for (auto& object3D : ListeScene)
			{
				object3D->Draw();

			}
			ListeSceneUI[InitUI()]->Draw();
			EndRenderSceneSpecific();
			
			
			return true;
		}


		virtual void Cleanup()
		{
			// Détruire le dispositif
			if (pDispositif)
			{
				delete pDispositif;
				pDispositif = nullptr;
			}
		}

		virtual int InitScene()
		{

			gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
			gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());
			gPvd = PxCreatePvd(*gFoundation);
			PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
			gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

			gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

			PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
			sceneDesc.gravity = PxVec3(0.0f, -30.0f, 0.0f);
			gDispatcher = PxDefaultCpuDispatcherCreate(2);
			sceneDesc.cpuDispatcher = gDispatcher;
			sceneDesc.filterShader = SimpleFilterShader;
			sceneDesc.simulationEventCallback = new CollisionHandler();
			sceneDesc.bounceThresholdVelocity = 10;
			sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
			gScene = gPhysics->createScene(sceneDesc);
			gScene->setFlag(PxSceneFlag::eENABLE_ACTIVE_ACTORS , true);
			gScene->setFlag(PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS, true);
			gMaterial = gPhysics->createMaterial(2.0f, 2.0f, 0.0f);
			PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
			if (pvdClient)
			{
				pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
				pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
				pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
			}

			// Initialisation des objets 3D - création et/ou chargement
			if (!InitObjets())
			{
				return 1;
			}
	
			// Initialisation des lumieres
			if (!InitLumieres())
			{
				return 1;
			}

			cameraController.Init(&m_MatView, &m_MatProj, &m_MatViewProj);

			return 0;
		}


		bool InitObjets()
		{

			TimeController::GetInstance().startTimer();

			//// Puis, il est ajouté à la scène
			vector<wstring> initMeshList = { L".\\modeles\\jin\\jin" };
				
#pragma region loading assets
			vector<GeometryInitData::GeometryStruct> jin =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"jin",L"jin",L".\\modeles\\jin\\releasetest\\")
			};
			vector<GeometryInitData::GeometryStruct> lp670 =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"Police car",L"Police car",L".\\modeles\\PoliceCar\\")
			};

			vector<GeometryInitData::GeometryStruct> vehicle =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"vehicle",L"vehicle",L".\\modeles\\vehicle\\"),
			};

			vector<GeometryInitData::GeometryStruct> driver =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"vehicle_driver",L"vehicle_driver",L".\\modeles\\vehicle\\"),
			};

			vector<GeometryInitData::GeometryStruct> cannon =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"vehicle_cannon",L"vehicle_cannon",L".\\modeles\\vehicle\\",false),
			};

			vector<GeometryInitData::GeometryStruct> rock_bigmama =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"lod0_rock_bigmama",L"lod0_rock_bigmama",L".\\modeles\\props\\rocks\\rock_bigmama\\",150),
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"lod1_rock_bigmama",L"lod1_rock_bigmama",L".\\modeles\\props\\rocks\\rock_bigmama\\",300),
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"lod2_rock_bigmama",L"lod2_rock_bigmama",L".\\modeles\\props\\rocks\\rock_bigmama\\",MAXINT32)
			};

			vector<GeometryInitData::GeometryStruct> teleport =
			{ 
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"Teleport",L"Teleport",L".\\modeles\\terrain\\CircuitGeometry\\Teleport\\",false),
			};

			vector<GeometryInitData::GeometryStruct> tree01 =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"tree01_lod0",L"tree01_lod0",L".\\modeles\\tree\\tree01\\"),
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"tree01_lod1",L"tree01_lod1",L".\\modeles\\tree\\tree01\\")
			};

			vector<GeometryInitData::GeometryStruct> terrain =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"wasteland",L"wasteland",L".\\modeles\\terrain\\CircuitGeometry\\",MAXINT32,true),
			};

			vector<GeometryInitData::GeometryStruct> terrain2 =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"Forest",L"Forest",L".\\modeles\\terrain\\CircuitGeometry\\ForestTerrain\\",MAXINT32,true),
			};

			vector<GeometryInitData::GeometryStruct> mix =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"lp670",L"lp670",L".\\modeles\\lp670\\"),
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"jin",L"jin",L".\\modeles\\jin\\")
			};

			vector<GeometryInitData::GeometryStruct> terrainSnowy =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"IcedPickedMountain_Terrain",L"IcedPickedMountain_Terrain",L".\\modeles\\terrain\\CircuitGeometry\\snowyMountain\\",MAXINT32,true),
			};

			vector<GeometryInitData::GeometryStruct> terrainIced =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"IcedPickedMountain_IcyTerrain",L"IcedPickedMountain_IcyTerrain",L".\\modeles\\terrain\\CircuitGeometry\\snowyMountain\\",MAXINT32,true),
			};
			

			vector<GeometryInitData::GeometryStruct> trees =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"ForetTree",L"ForetTree",L".\\modeles\\terrain\\CircuitGeometry\\ForestTerrain\\Obstacle\\",MAXINT32,true),
			};

			vector<GeometryInitData::GeometryStruct> wood =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"Wood",L"Wood",L".\\modeles\\terrain\\CircuitGeometry\\ForestTerrain\\Obstacle\\",MAXINT32,true),
			};

			vector<GeometryInitData::GeometryStruct> monstreBody =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"MonstreBody",L"MonstreBody",L".\\modeles\\Monstre\\",MAXINT32,true),
			};

			vector<GeometryInitData::GeometryStruct> monstreBladesBack =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"MonstreBladesBack",L"MonstreBladesBack",L".\\modeles\\Monstre\\",MAXINT32,true),
			};

			vector<GeometryInitData::GeometryStruct> monstreBladesFront =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"MonstreBladesFront",L"MonstreBladesFront",L".\\modeles\\Monstre\\",MAXINT32,true),
			};

			PhysicalInitData terrainPhysInitData2 = PhysicalInitData(TypeGeo::eTRIANGLEMESH, false, 0, 30, 0, PxVec3(1.0f), L"Forest", 0.8f, 0.8f, 0.0f);

			unique_ptr<GameObject> pterrain2 = std::make_unique<GameObject>(
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 30.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData(terrain2),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				terrainPhysInitData2
				);

			ListeScene.push_back(std::move(pterrain2));

			unique_ptr<GameObject> pterrainsnowy = std::make_unique<GameObject>(
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 30.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData(terrainSnowy),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				PhysicalInitData(TypeGeo::eTRIANGLEMESH, false, 0, 30, 0, PxVec3(1.0f), L"IcedPickedMountain_Terrain", 0.8f, 0.8f, 0.0f)
				);

			ListeScene.push_back(std::move(pterrainsnowy));

			unique_ptr<GameObject> pterrainIced = std::make_unique<GameObject>(
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 30.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData(terrainIced),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				PhysicalInitData(TypeGeo::eTRIANGLEMESH, false, 0, 30, 0, PxVec3(1.0f), L"IcedPickedMountain_IcyTerrain", 0.2f, 0.2f, 0.0f)
				);

			ListeScene.push_back(std::move(pterrainIced));

			unique_ptr<GameObject> pTrees = std::make_unique<GameObject>(
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 30.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData(trees),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				PhysicalInitData(TypeGeo::eTRIANGLEMESH, false, 0, 30, 0, PxVec3(1.0f), L"ForetTree", 0.8f, 0.8f, 0.0f)
				);

			ListeScene.push_back(std::move(pTrees));

			unique_ptr<GameObject> pWood = std::make_unique<GameObject>(
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 30.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData(wood),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				PhysicalInitData(TypeGeo::eTRIANGLEMESH, false, 0, 30, 0, PxVec3(1.0f), L"Wood", 0.8f, 0.8f, 0.0f)
				);

			ListeScene.push_back(std::move(pWood));


			unique_ptr<GameObject> pterrain = std::make_unique<GameObject>(
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 30.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData(terrain),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				PhysicalInitData(TypeGeo::eTRIANGLEMESH, false, 0, 30, 0, PxVec3(1.0f), L"wasteland", 0.8f, 0.8f, 0.0f)
				);

			ListeScene.push_back(std::move(pterrain));

			
			
			//skybox
			vector<GeometryInitData::GeometryStruct> skybox =
			{
				GeometryInitData::GeometryStruct(GeometryInitData::MESH,L"skybox",L"skybox",L".\\modeles\\skybox\\",MAXINT32,true),
			};
			ListeScene.push_back(std::make_unique<GameObject>(
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData(skybox),
				"PlaceHolderMaterialInstance"s,
				pDispositif
				));

			
			ListeScene.push_back(std::make_unique<GameObject>(
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(0.f, 30.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData(teleport),
				"PlaceHolderMaterialInstance"s,
				pDispositif
				));

			// rock 1 
			ListeScene.push_back( std::make_unique<GameObject>(
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(144.053f, 66.299f, -150.111f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData(rock_bigmama),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				PhysicalInitData(TypeGeo::eSPHERE, false, 144.053f, 66.299f, -150.111f, PxVec3(20, 0, 0), L"NONE", 0.8f, 0.8f, 0.0f)
				));
			
			//rock 2
			ListeScene.push_back(std::make_unique<GameObject>(
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(180.053f, 65.299f, -190.111f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData(rock_bigmama),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				PhysicalInitData(TypeGeo::eSPHERE, false, 180.053f, 65.299f, -190.111f, PxVec3(20, 0, 0), L"NONE", 0.8f, 0.8f, 0.0f)
				));

				// rock 3
				ListeScene.push_back(std::make_unique<GameObject>(
					XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 50.0f, -725.996f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
					GeometryInitData(rock_bigmama),
					"PlaceHolderMaterialInstance"s,
					pDispositif,
					PhysicalInitData(TypeGeo::eSPHERE, false, 0.0f, 50.0f, -725.996f, PxVec3(20, 0, 0), L"NONE", 0.8f, 0.8f, 0.0f)
					));


			// vehicle
			unique_ptr<Vehicule> pVehicule = std::make_unique<Vehicule>(
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 50.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData(vehicle),
				GeometryInitData(driver),
				GeometryInitData(cannon),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				PhysicalInitData(TypeGeo::eCONVEXMESH, true, 0, 50.0, 0, PxVec3(2.00f, 0.5f, 0.0f), L"", 0.5f, 0.5f, 0.0f, 300, 300)
				);
			vehicule = pVehicule.get();
			ListeScene.push_back(std::move(pVehicule));

			CreateTriggerZones();
			
			// billboard background
			ListeScene.push_back(std::make_unique<GameObject>(
				XMFLOAT4(5000.0f, 5000.0f, 5000.0f, 1.0f), XMFLOAT4(0.0f, 2000.0f, 5000.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData
				(
					{

					GeometryInitData::GeometryStruct(
						GeometryInitData::BILLBOARD,
						L"billboard_test",
						L"billboard_test",
						L".\\modeles\\billBoards\\",
						MAXINT32,
						false,
						false,
						{
							CMaterialData(".\\modeles\\billBoards\\billboard_test.dds","Mat_billboard_test")
						}
					)
					}
				),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				PhysicalInitData(TypeGeo::eSPHERE, false, 0.0f, 50.0f, -725.996f, PxVec3(20, 0, 0), L"NONE", 0.8f, 0.8f, 0.0f)
				));

			//terrain background
			float yscaling = 1.0f;
			float xzscaling = 0.505f;
			ListeScene.push_back(std::make_unique<GameObject>(
				XMFLOAT4(9.1429 * xzscaling, 5.660f* yscaling, 9.1429 * xzscaling, 1.0f), XMFLOAT4(-1365.499f, 30.0f+25.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData
				(
					{

					GeometryInitData::GeometryStruct(
						GeometryInitData::TERRAIN,
						L"heightMap_TerrainMain",
						L"heightMap_TerrainMain",
						L".\\modeles\\terrain\\HightMap\\",
						MAXINT32,
						false,
						false,
						{
							CMaterialData("sand.dds","sand",1000.0f,1000.0f),
							CMaterialData(".\\modeles\\terrain\\HightMap\\TextureHeightMap\\cliff.dds","cliff",1000.0f,1000.0f),
							CMaterialData(".\\modeles\\terrain\\HightMap\\TextureHeightMap\\TerrainMask.dds","TerrainMask",1000.0f,1000.0f)							
						},
						L".tif"
					)
					}
				),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				PhysicalInitData(TypeGeo::eSPHERE, false, 0.0f, 50.0f, -725.996f, PxVec3(20, 0, 0), L"NONE", 0.8f, 0.8f, 0.0f)
				));
			
			//terrain Moon
			ListeScene.push_back(std::make_unique<GameObject>(
				XMFLOAT4(1.f, 5.f, 1.f, 1.0f), XMFLOAT4(0.0f, 2200.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData
				(
					{
					GeometryInitData::GeometryStruct(
						GeometryInitData::TERRAIN,
						L"Moon",
						L"Moon",
						L".\\modeles\\terrain\\HightMap\\",
						MAXINT32,
						true,
						false,
						{
							
							CMaterialData(".\\modeles\\terrain\\HightMap\\TextureHeightMap\\sand.dds","sand",5.0f,5.0f),
							CMaterialData(".\\modeles\\terrain\\HightMap\\TextureHeightMap\\Montagne.dds","Montagne",5.0f,5.0f),
							CMaterialData(".\\modeles\\terrain\\HightMap\\TextureHeightMap\\Moon.dds","Moon",5000.0f,5000.0f)
						}
					)
					}
				),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				PhysicalInitData(TypeGeo::eTRIANGLEMESH, false, 0.0f, 2200.0f, 0.f, PxVec3(1.f, 5.f, 1.f), L"Moon", 0.8f, 0.8f, 0.0f)
				));
			
			unique_ptr<Monstres> pMonstre = std::make_unique<Monstres>(
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, -330.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
				GeometryInitData(monstreBody),
				GeometryInitData(monstreBladesBack),
				GeometryInitData(monstreBladesFront),
				"PlaceHolderMaterialInstance"s,
				pDispositif,
				PhysicalInitData(TypeGeo::eMONSTRE, true, 0, -330, 0, PxVec3(2,2,2), L"MonstreBody", 0.8f, 0.8f, 0.0f)
				);
			monstres = pMonstre.get();
			ListeScene.push_back(std::move(pMonstre));

#pragma endregion
			CreateTriggerZones();

			SoundController::GetInstance().playMenuSoundtrack();
			return true;
		}

		void CreateTriggerZones()
		{
			unique_ptr<DeadZone> pDeadZoneGeneral = std::make_unique<DeadZone>(PhysicalInitData(TypeGeo::eDEADZONE, false, 0.f, -50.f, 0.f, PxVec3(10000.f, 0.1f, 10000.f)));
			physicalScene.push_back(std::move(pDeadZoneGeneral));

			unique_ptr<DeadZone> pDeadZoneMoon = std::make_unique<DeadZone>(PhysicalInitData(TypeGeo::eDEADZONE, false, -8.f, 2180.f, 10.f, PxVec3(250.f, 0.1f, 250.f)));
			physicalScene.push_back(std::move(pDeadZoneMoon));

			unique_ptr<DeadZone> pDeadZoneForest = std::make_unique<DeadZone>(PhysicalInitData(TypeGeo::eDEADZONE, false, -1744.f, 9.f, -969.f, PxVec3(100.f, 0.1f, 100.f)));
			physicalScene.push_back(std::move(pDeadZoneForest));

			unique_ptr<DeadZone> pDeadZoneWater = std::make_unique<DeadZone>(PhysicalInitData(TypeGeo::eDEADZONE, false, -2380.f, -45.f, -800.f, PxVec3(300.f, 0.1f, 300.f)));
			physicalScene.push_back(std::move(pDeadZoneWater));

			unique_ptr<DeadZone> pDeadZoneIce= std::make_unique<DeadZone>(PhysicalInitData(TypeGeo::eDEADZONE, false, -2725.f, -5.f, 155.f, PxVec3(200.f, 0.1f, 200.f)));
			physicalScene.push_back(std::move(pDeadZoneIce));

			unique_ptr<DeadZone> pDeadZoneIcedCave1= std::make_unique<DeadZone>(PhysicalInitData(TypeGeo::eDEADZONE, false, -3081.f, -20.f, 20.f, PxVec3(200.f, 0.1f, 200.f)));
			physicalScene.push_back(std::move(pDeadZoneIcedCave1));

			unique_ptr<DeadZone> pDeadZoneIcedCave2 = std::make_unique<DeadZone>(PhysicalInitData(TypeGeo::eDEADZONE, false, -3352.f, 5.f, -59.f, PxVec3(75.f, 0.1f, 150.f)));
			physicalScene.push_back(std::move(pDeadZoneIcedCave2));

			unique_ptr<DeadZone> pDeadZoneStupidPlayer = std::make_unique<DeadZone>(PhysicalInitData(TypeGeo::eDEADZONE, false, -2693.f, 20.f, -417.f, PxVec3(150.f, 0.1f, 150.f)));
			physicalScene.push_back(std::move(pDeadZoneStupidPlayer));

			unique_ptr<TriggerZoneCave> pTriggerZoneCave1 = std::make_unique<TriggerZoneCave>(PhysicalInitData(TypeGeo::eDEADZONE, false, 50.f, 5.f, -550.f, PxVec3(50.f, 25.f, 0.1f)));
			physicalScene.push_back(std::move(pTriggerZoneCave1));

			unique_ptr<TriggerZoneCave> pTriggerZoneCave2 = std::make_unique<TriggerZoneCave>(PhysicalInitData(TypeGeo::eDEADZONE, false, -144.f, 50.f, -833.f, PxVec3(75.f, 25.f, .1f),L"",0.f,0.f,0.f,0.f,0.f,PxVec4(0.f,0.998f,0.f,0.057f)));
			physicalScene.push_back(std::move(pTriggerZoneCave2));

			unique_ptr<TriggerZoneCave> pTriggerZoneCave3 = std::make_unique<TriggerZoneCave>(PhysicalInitData(TypeGeo::eDEADZONE, false, 43.f, 35.f, -648.f, PxVec3(50.f, 0.1f, 75.f)));
			physicalScene.push_back(std::move(pTriggerZoneCave3));

			unique_ptr<TriggerZoneCave> pTriggerZoneCave4 = std::make_unique<TriggerZoneCave>(PhysicalInitData(TypeGeo::eDEADZONE, false, -2317.f, 37.f, -872.f, PxVec3(50.f, 25.f, 0.1f), L"", 0.f, 0.f, 0.f, 0.f, 0.f, PxVec4(0.f, -0.040f, 0.f, 0.999f)));
			physicalScene.push_back(std::move(pTriggerZoneCave4));

			unique_ptr<TriggerZoneCave> pTriggerZoneCave5 = std::make_unique<TriggerZoneCave>(PhysicalInitData(TypeGeo::eDEADZONE, false, -2584.f, -20.f, -533.f, PxVec3(75.f, 25.f, .1f), L"", 0.f, 0.f, 0.f, 0.f, 0.f, PxVec4(0.f,0.812f, 0.f, 0.584f)));
			physicalScene.push_back(std::move(pTriggerZoneCave5));

			unique_ptr<TriggerZoneCave> pTriggerZoneCave6 = std::make_unique<TriggerZoneCave>(PhysicalInitData(TypeGeo::eDEADZONE, false, -2953.f, 15.f, 195.f, PxVec3(75.f, 25.f, 0.1f), L"", 0.f, 0.f, 0.f, 0.f, 0.f, PxVec4(0.f, 0.656f, 0.f, 0.755f)));
			physicalScene.push_back(std::move(pTriggerZoneCave6));

			unique_ptr<TriggerZoneCave> pTriggerZoneCave7 = std::make_unique<TriggerZoneCave>(PhysicalInitData(TypeGeo::eDEADZONE, false, -3449.f, 37.f, -50.f, PxVec3(50.f, 25.f, 0.1f), L"", 0.f, 0.f, 0.f, 0.f, 0.f, PxVec4(0.f, 0.656f, 0.f, 0.755f)));
			physicalScene.push_back(std::move(pTriggerZoneCave7));

			unique_ptr<TriggerZoneTp> pTriggerZoneMoon = std::make_unique<TriggerZoneTp>(PhysicalInitData(TypeGeo::eDEADZONE, false, -3537.f, 36.f, -395.f, PxVec3(50.f, 25.f, 0.1f), L"", 0.f, 0.f, 0.f, 0.f, 0.f, PxVec4(0.f, 0.304f, 0.f, 0.953f)));
			physicalScene.push_back(std::move(pTriggerZoneMoon));

			unique_ptr<TriggerZoneTp> pTriggerZoneMoon2 = std::make_unique<TriggerZoneTp>(PhysicalInitData(TypeGeo::eDEADZONE, false, 194.f, 2222.f, 23.f, PxVec3(50.f, 25.f, 0.1f), L"", 0.f, 0.f, 0.f, 0.f, 0.f, PxVec4(0.f, 0.656f, 0.f, 0.755f)));
			physicalScene.push_back(std::move(pTriggerZoneMoon2));

		}

		bool InitLumieres() {
			// sun
			Light pointLightSun = Light(
				"pointSun"s,
				XMVectorSet(.01f, .01f, .015f, 1.0f),
				XMVectorSet(0.8f, 0.75f, 0.70f, 1.0f),
				XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f),
				LightType::POINTLIGHT,
				XMVectorSet(-7162.746f*1.5,1000.0f*1.5f, 0.0f, 1.0f),
				XMVectorSet(1.0f, 0.0f, 1000000000.0f, 0.0f),
				XMVectorSet(0,0,0,1),true
			); 
			
			//lumieres de test
			Light directLightSun = Light(
				"directSun"s,
				XMVectorSet(.40f, .40f, .50f, 1.0f),
				XMVectorSet(0.8f, 0.75f, 0.70f, 1.0f),
				XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f),
				LightType::DIRECTIONAL,
				//XMVectorSet(-7162.746f * 1.5f, 1000.0f * 1.5f, 0.0f, 1.0f),
				XMVectorSet(-8585.597f, 1388.621f, -3295.487f, 1.0f),
				XMVectorSet(1000.0f, 0.0f, 100000.0f, 0.0f),
				XMVectorSet(0.907630f, -0.1847f, 0.37692135574476743f, 1.0f),
				true
			);

			Light testPointSun = Light(
				"testPointSun"s,
				XMVectorSet(.1f, .1f, .15f, 1.0f),
				XMVectorSet(0.8f, 0.75f, 0.70f, 1.0f),
				XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f),
				LightType::POINTLIGHT,
				XMVectorSet(0.0f, 100.0f, -47.f, 1.0f),
				XMVectorSet(100.0f, 0.0f, 20000.0f, 0.0f),
				XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
				true
			);

			Light pointLightCaveGreenGlow = Light(
				"caveGreen"s,
				XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
				XMVectorSet(0.0f, 0.8f, 0.2f, 0.5f),
				XMVectorSet(0.0f, 0.5f, 0.0f, 0.8f),
				LightType::POINTLIGHT,
				XMVectorSet(22.0f, 31.0f, -800.0f, 1.0f),
				XMVectorSet(40.0f, 0.0f, 100.0f, 1.0f)
			);
			pointLightCaveGreenGlow.isActive = 1;

			listeLumieres.push_back(directLightSun);
			listeLumieres.push_back(pointLightCaveGreenGlow);

			//initialize the light matrices
			for (auto& light : listeLumieres) {
				if (light.lightType != DIRECTIONAL) continue;
				light.computeViewMatrix();
				light.computeProjMatrix(FOV, pDispositif->getRatioDAspect(), 0.1f, FARPLANE * 100, 2048.0f, 2048.0f);
				light.getViewProjMatrix(); //computes the matrix when called if needed, called here to precompute the matrix
			}

			return true;
		}

		bool AnimeScene(float tempsEcoule)
		{
			if(vehicule){
			// Prendre en note le statut du clavier
			GestionnaireDeSaisie.StatutClavier();
			// Prendre en note l’état de la souris
			GestionnaireDeSaisie.SaisirEtatSouris();
			// Gerer les inputs
			inputController.HandleKeyboardInput();
			inputController.HandleMouseInput();
			//bouger la camera
			cameraController.updateCamera();
			}

			stepPhysics();

			for (auto& object3D : ListeScene)
			{
				object3D->Anime(tempsEcoule);
			}
			
			ListeSceneUI[InitUI()]->Draw();
		

			return true;
		}

	protected:
		// Variables pour le temps de l'animation
		int64_t TempsSuivant;
		int64_t TempsCompteurPrecedent;

		// Le dispositif de rendu
		TClasseDispositif* pDispositif;

		// La seule scène
		
		std::vector<Light> listeLumieres;

		// Les matrices
		XMMATRIX m_MatView;
		XMMATRIX m_MatProj;
		XMMATRIX m_MatViewProj;

		CDIManipulateur GestionnaireDeSaisie;
		InputController inputController;
		

		// Le gestionnaire de texture
		CGestionnaireDeTextures TexturesManager;
		CMeshInstanceManager MeshInstanceManager;
		ShaderInstanceManager ShaderInstanceManager;

		std::wstring str;
		std::unique_ptr<Gdiplus::Font> pPolice;

		std::vector<std::unique_ptr<CObjet3D>> ListeScene;
		std::vector<std::unique_ptr<CObjet3D>> ListeSceneUI;

		enum class Option {
			Audio,
			Video,
			Difficulte,
			Touche,
			None
		};

		//texture de rendu (pour ombre, rendu intermediaire, etc)
		RenderTexture renderTexture;
	public:
		

		std::unique_ptr<CAfficheurTexte> pTexteChrono;
		std::unique_ptr<CAfficheurTexte> pTexteSpeed;
		std::unique_ptr<CAfficheurTexte> pTexteMonstre;
		std::unique_ptr<CAfficheurTexte> pTexteTours;
		//menu texts 
		std::unique_ptr<CAfficheurTexte> pTexteReprendre;
		std::unique_ptr<CAfficheurTexte> pTexteReset;
		std::unique_ptr<CAfficheurTexte> pTexteOptions;
		std::unique_ptr<CAfficheurTexte> pTexteQuitter;
		
		

		int selection = 0;
		double tempsEntreLumiere;
		double tempsEntreTP;
		Vehicule* vehicule;
		Monstres* monstres;
		float difficulte = 0.8f;
		CameraController cameraController;
		
		Option activeOption = Option::None;
		void resetOption() { activeOption = Option::None; }
		void setOptionAudio() { activeOption = Option::Audio; }
		void setOptionDifficulte() { activeOption = Option::Difficulte; }
		void setOptionVideo() { activeOption = Option::Video; }
		void setOptionTouche() { activeOption = Option::Touche; }
		bool optionIsNone() { return activeOption == Option::None; }
		bool optionIsAudio() { return activeOption == Option::Audio; }
		bool optionIsVideo() { return activeOption == Option::Video; }
		bool optionIsDifficulte() { return activeOption == Option::Difficulte; }
		bool optionIsTouche() { return activeOption == Option::Touche; }

		std::vector<unique_ptr<ObjectPhysicalInstance>> physicalScene;
	};

} // namespace PM3D
