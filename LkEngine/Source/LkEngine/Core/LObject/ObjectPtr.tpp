namespace LkEngine {

	template<typename T>
	template<typename... TArgs>
	TObjectPtr<T> TObjectPtr<T>::Create(TArgs&&... Args)
	{
		return TObjectPtr<T>(new T(std::forward<TArgs>(Args)...));
	}

}