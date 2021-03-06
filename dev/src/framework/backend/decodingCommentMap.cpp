#include "build.h"
#include "decodingMemoryMap.h"
#include "decodingCommentMap.h"
#include "internalFile.h"

namespace decoding
{

	CommentMap::CommentMap( class MemoryMap* map )
		: m_memoryMap( map )
		, m_isModified( false )
	{
	}

	CommentMap::~CommentMap()
	{
	}

	const char* CommentMap::GetComment( const uint32 addr ) const
	{
		// read comment data
		if ( m_memoryMap->GetMemoryInfo( addr ).HasComment() )
		{
			TCommentMap::const_iterator it = m_commentMap.find( addr );
			if ( it != m_commentMap.end() )
			{
				return (*it).second.c_str();
			}
		}

		// no comment
		return "";
	}

	void CommentMap::SetComment( const uint32 addr, const char* comment )
	{
		if ( !comment || !comment[0] )
		{
			ClearComment( addr );
		}
		else if ( m_memoryMap->IsValidAddress( addr ) )
		{
			m_memoryMap->SetMemoryBlockType( ILogOutput::DevNull(), addr, (uint32)MemoryFlag::HasComment, 0 );

			// modify only when needed
			TCommentMap::const_iterator it = m_commentMap.find( addr );
			if ( it == m_commentMap.end() || (*it).second != comment )
			{
				m_commentMap[ addr ] = comment;
				m_isModified = true;
			}
		}
	}

	void CommentMap::ClearComment( const uint32 addr )
	{
		if ( m_memoryMap->IsValidAddress( addr ) )
		{
			m_memoryMap->SetMemoryBlockType( ILogOutput::DevNull(), addr, 0, (uint32)MemoryFlag::HasComment );

			TCommentMap::iterator it = m_commentMap.find( addr );
			if ( it != m_commentMap.end() )
			{
				m_commentMap.erase( it );
				m_isModified = true;
			}
		}
	}

	bool CommentMap::Save( ILogOutput& log, class IBinaryFileWriter& writer ) const
	{
		CBinaryFileChunkScope chunk( writer, eFileChunk_CommentMap );
		writer << m_commentMap;

		m_isModified = false;
		return true;
	}

	bool CommentMap::Load( ILogOutput& log, class IBinaryFileReader& reader )
	{
		CBinaryFileChunkScope chunk( reader, eFileChunk_CommentMap );
		reader >> m_commentMap;

		m_isModified = false;
		return true;
	}

} // decoding