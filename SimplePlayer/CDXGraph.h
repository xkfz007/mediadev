/* 雷霄骅
 * 中国传媒大学/数字电视技术
 * leixiaohua1020@126.com
 *
 */
// CDXGraph.h


#ifndef __H_CDXGraph__
#define __H_CDXGraph__

// Filter graph notification to the specified window
#define WM_GRAPHNOTIFY  (WM_USER+20)

class CDXGraph
{
private:
	//各种DirectShow接口
	IGraphBuilder *     mGraph;  
	IMediaControl *		mMediaControl;
	IMediaEventEx *		mEvent;
	IBasicVideo *		mBasicVideo;
	IBasicAudio *		mBasicAudio;
	IVideoWindow  *		mVideoWindow;
	IMediaSeeking *		mSeeking;

	DWORD				mObjectTableEntry; 

public:
	CDXGraph();
	virtual ~CDXGraph();

public:
	//创建IGraphBuilder，使用CoCreateInstance
	virtual bool Create(void);
	//释放
	virtual void Release(void);
	virtual bool Attach(IGraphBuilder * inGraphBuilder);

	IGraphBuilder * GetGraph(void); // Not outstanding reference count
	IMediaEventEx * GetEventHandle(void);

	bool ConnectFilters(IPin * inOutputPin, IPin * inInputPin, const AM_MEDIA_TYPE * inMediaType = 0);
	void DisconnectFilters(IPin * inOutputPin);

	bool SetDisplayWindow(HWND inWindow);
	bool SetNotifyWindow(HWND inWindow);
	bool ResizeVideoWindow(long inLeft, long inTop, long inWidth, long inHeight);
	void HandleEvent(WPARAM inWParam, LPARAM inLParam);
	//各种操作
	bool Run(void);        // Control filter graph
	bool Stop(void);
	bool Pause(void);
	bool IsRunning(void);  // Filter graph status
	bool IsStopped(void);
	bool IsPaused(void);

	bool SetFullScreen(BOOL inEnabled);
	bool GetFullScreen(void);

	// IMediaSeeking
	bool GetCurrentPosition(double * outPosition);
	bool GetStopPosition(double * outPosition);
	bool SetCurrentPosition(double inPosition);
	bool SetStartStopPosition(double inStart, double inStop);
	bool GetDuration(double * outDuration);
	bool SetPlaybackRate(double inRate);

	// Attention: range from -10000 to 0, and 0 is FULL_VOLUME.
	bool SetAudioVolume(long inVolume);
	long GetAudioVolume(void);
	// Attention: range from -10000(left) to 10000(right), and 0 is both.
	bool SetAudioBalance(long inBalance);
	long GetAudioBalance(void);

	bool RenderFile(const char * inFile);
	bool SnapshotBitmap(const char * outFile);

private:
	void AddToObjectTable(void) ;
	void RemoveFromObjectTable(void);
	//各种QueryInterface，初始各种接口
	bool QueryInterfaces(void);
};

#endif // __H_CDXGraph__