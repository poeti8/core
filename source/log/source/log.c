/*
 *	Logger Library by Parra Studios
 *	Copyright (C) 2016 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>
 *
 *	A generic logger library providing application execution reports.
 *
 */

/* -- Headers -- */

#include <metacall/metacall-version.h>

#include <log/log.h>
#include <log/log_impl.h>
#include <log/log_singleton.h>

#include <stdio.h>

/* -- Methods -- */

int log_create(const char * name)
{
	log_impl impl = log_impl_create(name);

	if (impl == NULL)
	{
		return 1;
	}

	if (log_singleton_insert(log_impl_name(impl), impl) != 0)
	{
		log_impl_destroy(impl);

		return 1;
	}

	return 0;
}

int log_define(const char * name, log_policy policy)
{
	log_impl impl = log_singleton_get(name);

	enum log_aspect_id aspect_id = log_policy_aspect_id(policy);

	log_aspect aspect = log_impl_aspect(impl, aspect_id);

	if (aspect == NULL)
	{
		log_aspect_interface aspect_iface = log_impl_aspect_interface(aspect_id);

		/* TODO: review aspect constructor */

		aspect = log_aspect_create(impl, aspect_iface, NULL);

		if (aspect == NULL)
		{
			return 1;
		}

		log_impl_define(impl, aspect, aspect_id);
	}

	if (log_aspect_attach(aspect, policy) != 0)
	{
		return 1;
	}

	return 0;
}

int log_write_impl(const char * name, const size_t line, const char * func, const char * file, const enum log_level_id level, const char * message, ...)
{
	log_impl impl = log_singleton_get(name);

	struct log_record_ctor_type record_ctor;

	int result;

	va_list variable_args;

	va_start(variable_args, message);

	record_ctor.line = line;
	record_ctor.func = func;
	record_ctor.file = file;
	record_ctor.level = level;
	record_ctor.message = message;
	record_ctor.data = variable_args;

	result = log_impl_write(impl, &record_ctor);

	va_end(variable_args);

	return result;
}

int log_clear(const char * name)
{
	log_impl impl = log_singleton_get(name);

	if (impl == NULL)
	{
		return 1;
	}

	return log_impl_clear(impl);
}

int log_delete(const char * name)
{
	log_impl impl = log_singleton_remove(name);

	if (impl == NULL)
	{
		return 1;
	}

	return log_impl_destroy(impl);
}

void log_print_info()
{
	printf("Logger Library " METACALL_VERSION "\n");
	printf("Copyright (c) 2016 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>\n");

	#ifdef LOG_STATIC_DEFINE
		printf("Compiled as static library type\n");
	#else
		printf("Compiled as shared library type\n");
	#endif
}